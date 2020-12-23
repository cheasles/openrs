#pragma once

#include <stdint.h>

#include <map>
#include <vector>

#include "openrs/net/session.h"
#include "openrs/net/codec/packet.h"

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

class PacketHandler {
 public:
  virtual void Handle(openrs::net::codec::Packet& packet,
                      openrs::net::Session* session);

  virtual std::vector<openrs::net::codec::PacketType> HandledTypes() const {
    return {
        openrs::net::codec::PacketType::kHandshake,
    };
  };
};

}  // namespace global

}  // namespace handler

}  // namespace codec

}  // namespace net

}  // namespace openrs
