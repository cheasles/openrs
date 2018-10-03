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
        << " archive: " << std::to_string(::be32toh(*archive_id_ptr));

    try
    {
        const auto& store = openrs::manager::cache::CacheManager::get();
    }
    catch (const std::runtime_error& ex)
    {

    }
}
