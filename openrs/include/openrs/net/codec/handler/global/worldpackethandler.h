#pragma once

#include <stdint.h>

#include <map>
#include <vector>

#include "openrs/net/codec/handler/global/packethandler.h"
#include "openrs/net/codec/packet.h"
#include "openrs/net/session.h"

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

class WorldPacketHandler : public PacketHandler {
 public:
  void Handle(openrs::net::codec::Packet& packet,
              openrs::net::Session* session) override;
};

}  // namespace global
}  // namespace handler
}  // namespace codec
}  // namespace net
}  // namespace openrs
