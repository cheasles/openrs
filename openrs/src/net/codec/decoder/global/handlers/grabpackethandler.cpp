#include "OpenRS/net/codec/decoder/global/handlers/grabpackethandler.h"

#include <endian.h>

#include <string>

#include <integer.h>
#include <rsa.h>
#include <whrlpool.h>
#include "Common/log.h"

#include "OpenRS/net/io/buffer.h"
#include "OpenRS/manager/configmanager.h"
#include "OpenRS/manager/cache/cachemanager.h"
#include "OpenRS/manager/cache/grabmanager.h"

bool PackCacheData(const uint8_t& kIndexId, const uint32_t& kArchiveId,
                   const uint8_t& kSettings, const uint32_t& kRealLength,
                   openrs::net::io::Buffer& cache_data, openrs::net::io::Buffer* output)
{
    output->PutData(kIndexId);
    output->PutDataBE(kArchiveId);
    output->PutData(kSettings);
    output->PutDataBE(kRealLength);
    uint8_t* cache_data_data_ptr = nullptr;
    for (uint32_t i = 0; i < cache_data.size(); ++i)
    {
        if (!cache_data.GetData(&cache_data_data_ptr))
        {
            openrs::common::Log(openrs::common::Log::LogLevel::kError)
                << "Failed to read cache data.";
            return false;
        }
        if (output->size() % 512 == 0)
            output->PutData(255);
        output->PutData(*cache_data_data_ptr);
    }

    return true;
}

void SendUKeys(openrs::net::Client* client)
{
    openrs::net::io::Buffer ukeys_data_packet_buffer;
    const auto& cache = openrs::manager::cache::CacheManager::get().cache();

    if (cache->GetTypeCount() > static_cast<size_t>(std::numeric_limits<uint8_t>::max()))
    {
        openrs::common::Log(openrs::common::Log::LogLevel::kError)
            << "Too many cache indexes (" << cache->GetTypeCount() << ").";
        return;
    }

    ukeys_data_packet_buffer.PutData(static_cast<uint8_t>(cache->GetTypeCount()));
    for (uint32_t index = 0; index < cache->GetTypeCount(); ++index)
    {
        try
        {
            const auto& store = cache->GetStore(index);
            ukeys_data_packet_buffer.PutDataBE(store.crc());
            ukeys_data_packet_buffer.PutDataBE(store.reference_table().revision());
            ukeys_data_packet_buffer.insert(ukeys_data_packet_buffer.cend(),
                store.whirlpool().cbegin(), store.whirlpool().cend());
        }
        catch (const std::runtime_error& ex)
        {
            ukeys_data_packet_buffer.PutData<uint32_t>(0);
            ukeys_data_packet_buffer.PutData<uint32_t>(0);
            ukeys_data_packet_buffer.resize(ukeys_data_packet_buffer.size() + 64);
        }
    }

    // Whirlpool digest with leading zero.
    std::array<uint8_t, 65> ukeys_hash = { 0 };
    CryptoPP::Whirlpool().CalculateDigest(ukeys_hash.data() + 1, ukeys_data_packet_buffer.data(),
        ukeys_data_packet_buffer.size());

    // Encrypt the whirlpool hash with RSA.
    CryptoPP::RSA::PublicKey rsa_function;
    const auto grab_config = openrs::manager::ConfigManager::get()["grab"];
    CryptoPP::Integer private_exponent(
        grab_config["private_exponent"].get<std::string>().c_str());
    CryptoPP::Integer modulus(
        grab_config["modulus"].get<std::string>().c_str());
    rsa_function.Initialize(modulus, private_exponent);
    const auto encrypted_hash_int = rsa_function.ApplyFunction(
        CryptoPP::Integer(ukeys_hash.data(), ukeys_hash.size()));
    std::vector<uint8_t> encrypted_hash;
    // Prefix encrypted hash with zero.
    encrypted_hash.resize(encrypted_hash_int.MinEncodedSize() + 1);
    encrypted_hash[0] = 0;
    encrypted_hash_int.Encode(encrypted_hash.data() + 1, encrypted_hash.size() - 1);
    ukeys_data_packet_buffer.insert(ukeys_data_packet_buffer.cend(),
        encrypted_hash.cbegin(), encrypted_hash.cend());

    // Send the result to the client.
    openrs::net::io::Buffer packet_buffer;
    if (!PackCacheData(255, 255, 0,
                       static_cast<uint32_t>(ukeys_data_packet_buffer.size()),
                       ukeys_data_packet_buffer, &packet_buffer)) {
        return;
    }

    openrs::net::codec::Packet cache_data_packet;
    cache_data_packet.type = openrs::net::codec::PacketType::kGrabCache;
    cache_data_packet.data = packet_buffer;
    client->Send(cache_data_packet);
}

void openrs::net::codec::decoder::global::handlers::GrabPacketHandler::Handle(
    openrs::net::codec::Packet& packet,
    openrs::net::Client* client)
{
    while (packet.data.position() != packet.data.size())
    {
        uint8_t* priority_ptr = nullptr;
        uint8_t* index_id_ptr = nullptr;
        uint32_t* archive_id_ptr = nullptr;
        bool priority = false;
        if (!packet.data.GetData(&priority_ptr) ||
            !packet.data.GetData(&index_id_ptr) ||
            !packet.data.GetData(&archive_id_ptr))
        {
            return;
        }

        common::Log(common::Log::LogLevel::kDebug)
            << "Client " << client->socket().getSocketId()
            << " requested cache"
            << " index: " << std::to_string(*index_id_ptr)
            << " archive: " << ::be32toh(*archive_id_ptr)
            << " priority: " << std::to_string(*priority_ptr);

        switch (*priority_ptr) {
            case 1:
                priority = true;
                break;
            case 2:
            case 3:
                continue;
            case 4:
                common::Log(common::Log::LogLevel::kDebug)
                    << "Client " << client->socket().getSocketId()
                    << " requested unsupported priority "
                    << std::to_string(*priority_ptr);
                continue;
            case 7:
                client->set_status(ClientStatus::kDisconnected);
                return;
        }

        if (::be32toh(*archive_id_ptr) == 255 && *index_id_ptr == 255)
        {
            SendUKeys(client);
            continue;
        }

        io::Buffer cache_data;
        try
        {
            const auto& store = openrs::manager::cache::CacheManager::get();
            if (!store.GetArchiveData(*index_id_ptr, ::be32toh(*archive_id_ptr), &cache_data))
            {
                common::Log(common::Log::LogLevel::kWarning)
                    << "Failed to retrieve cache for client " << client->socket().getSocketId();
                continue;
            }
        }
        catch (const std::runtime_error& ex)
        {
            common::Log(common::Log::LogLevel::kError)
                << "An exception occurred when trying to grab the cache for a client: " << ex.what();
            return;
        }

        uint8_t* cache_data_compression_ptr = nullptr;
        uint32_t* cache_data_length_ptr = nullptr;

        if (!cache_data.GetData(&cache_data_compression_ptr) ||
            !cache_data.GetData(&cache_data_length_ptr))
        {
            common::Log(common::Log::LogLevel::kError)
                << "Failed to read cache data.";
            return;
        }

        uint8_t settings = *cache_data_compression_ptr;
        if (priority)
            // com.rs.net.encoders.GrabPacketsEncoder.getArchivePacketData(int, int, boolean)
            settings |= 0x80;

        io::Buffer cache_data_packet_buffer;
        cache_data_packet_buffer.PutDataBE(*index_id_ptr);
        cache_data_packet_buffer.PutDataBE(*archive_id_ptr);
        cache_data_packet_buffer.PutDataBE(settings);
        cache_data_packet_buffer.PutDataBE(*cache_data_length_ptr);
        uint32_t real_length = *cache_data_compression_ptr == 0
                                ? ::be32toh(*cache_data_length_ptr) + 4
                                : ::be32toh(*cache_data_length_ptr);

        common::Log(common::Log::LogLevel::kDebug)
            << "Real cache length " << real_length;
        uint8_t* cache_data_data_ptr = nullptr;
        for (uint32_t i = 0; i < real_length; ++i)
        {
            if (!cache_data.GetData(&cache_data_data_ptr))
            {
                common::Log(common::Log::LogLevel::kError)
                    << "Failed to read cache data.";
                return;
            }
            if (cache_data_packet_buffer.size() % 512 == 0)
                cache_data_packet_buffer.PutData(255);
            cache_data_packet_buffer.PutData(*cache_data_data_ptr);
        }
        Packet cache_data_packet;
        cache_data_packet.type = PacketType::kGrabCache;
        cache_data_packet.data = cache_data_packet_buffer;
        client->Send(cache_data_packet);
    }
}
