#pragma once

#include <stdint.h>

#include <map>
#include <vector>

#include "OpenRS/net/session.h"
#include "OpenRS/net/codec/handler/global/packethandler.h"
#include "OpenRS/net/codec/packet.h"

namespace openrs {

namespace net {

class Session;

}  // namespace net

}  // namespace openrs

namespace openrs {

namespace net {

namespace codec {

namespace handler {

namespace global {

class LoginPacketHandler : public PacketHandler {
 public:
  void Handle(openrs::net::codec::Packet& packet,
              openrs::net::Session* session) override;
};

}  // namespace global

}  // namespace handler

}  // namespace codec

}  // namespace net

}  // namespace openrs
