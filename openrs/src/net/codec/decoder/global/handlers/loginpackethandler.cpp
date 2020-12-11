#include "OpenRS/net/codec/decoder/global/handlers/loginpackethandler.h"

#include <endian.h>

#include <string>
#include "Common/log.h"

#include "OpenRS/net/io/buffer.h"
#include "OpenRS/manager/configmanager.h"
#include "OpenRS/manager/cache/cachemanager.h"
#include "OpenRS/manager/cache/grabmanager.h"

void openrs::net::codec::decoder::global::handlers::LoginPacketHandler::Handle(
    openrs::net::codec::Packet& packet,
    openrs::net::Client* client)
{
    if (PacketType::kLoginWorld == packet.type)
    {

    }
    else if (PacketType::kLoginLobby == packet.type)
    {

    }
}
