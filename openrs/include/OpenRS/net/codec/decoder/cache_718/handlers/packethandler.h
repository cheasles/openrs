#pragma once

#include <stdint.h>

#include <map>
#include <vector>

#include "OpenRS/net/client.h"
#include "OpenRS/net/codec/packet.h"

namespace openrs {

namespace net {

class Client;

}  // namespace net

}  // namespace openrs

namespace openrs {

namespace net {

namespace codec {

namespace decoder {

namespace cache_718 {

namespace handlers {

class PacketHandler {
 public:
  virtual void Handle(openrs::net::codec::Packet& packet,
                      openrs::net::Client* client);

  virtual std::vector<openrs::net::codec::PacketType> HandledTypes() const {
    return {
        openrs::net::codec::PacketType::kHandshake,
    };
  };
};

}  // namespace handlers

}  // namespace cache_718

}  // namespace decoder

}  // namespace codec

}  // namespace net

}  // namespace openrs
