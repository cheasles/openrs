#include "OpenRS/net/codec/decoder/global/handlers/grabpackethandler.h"

#include <endian.h>

#include <string>

#include "Common/log.h"

#include "OpenRS/net/io/buffer.h"
#include "OpenRS/manager/cache/cachemanager.h"
#include "OpenRS/manager/cache/grabmanager.h"

void openrs::net::codec::decoder::global::handlers::GrabPacketHandler::Handle(
    openrs::net::codec::Packet& packet,
    openrs::net::Client* client)
{
    while (packet.data.position() != packet.data.size())
    {
        uint8_t* priority_ptr = nullptr;
        uint8_t* index_id_ptr = nullptr;
        uint32_t* archive_id_ptr = nullptr;
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
            << " archive: " << ::be32toh(*archive_id_ptr);

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
        if (*priority_ptr)
            // com.rs.net.encoders.GrabPacketsEncoder.getArchivePacketData(int, int, boolean)
            settings |= 0x80;

        io::Buffer cache_data_packet_buffer;
        cache_data_packet_buffer.PutData(*index_id_ptr);
        cache_data_packet_buffer.PutData(*archive_id_ptr);
        cache_data_packet_buffer.PutData(settings);
        cache_data_packet_buffer.PutData(*cache_data_length_ptr);
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
