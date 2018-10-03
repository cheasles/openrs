#pragma once

#include <stdint.h>

#include <map>
#include <vector>

#include "OpenRS/net/client.h"
#include "OpenRS/net/codec/packet.h"
#include "OpenRS/net/codec/decoder/global/handlers/packethandler.h"

namespace openrs
{

namespace net
{

class Client;

}  // namespace net

}  // namespace openrs

namespace openrs
{

namespace net
{

namespace codec
{

namespace decoder
{

namespace global
{

namespace handlers
{

class GrabPacketHandler : public PacketHandler
{
public:
    void Handle(openrs::net::codec::Packet& packet,
        openrs::net::Client* client) override;
};

}  // namespace handlers

}  // namespace global

}  // namespace decoder

}  // namespace codec

}  // namespace net

}  // namespace openrs
