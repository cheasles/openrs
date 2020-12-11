#pragma once

#include <stdint.h>

#include <map>
#include <vector>

#include "OpenRS/net/client.h"
#include "OpenRS/net/codec/handler/global/packethandler.h"
#include "OpenRS/net/codec/packet.h"

namespace openrs {

namespace net {

class Client;

}  // namespace net

}  // namespace openrs

namespace openrs {

namespace net {

namespace codec {

namespace handler {

namespace global {

class GrabPacketHandler : public PacketHandler {
 public:
  void Handle(openrs::net::codec::Packet& packet,
              openrs::net::Client* client) override;
};


}  // namespace global

}  // namespace handler

}  // namespace codec

}  // namespace net

}  // namespace openrs
