#pragma once

#include <common/io/buffer.h>
#include <frozen/map.h>
#include <stdint.h>

#include <map>

#include "openrs/net/codec/packet.h"

namespace openrs {

namespace net {

namespace codec {

namespace decoder {

namespace global {

class Decoder {
 private:
  static constexpr frozen::map<uint8_t, PacketType, 3> code_mapping_{
      {15, PacketType::kHandshake},
      {14, PacketType::kLogin},
      {6, PacketType::kGrabCache},
  };

 public:
  virtual bool Decode(openrs::common::io::Buffer<>& buffer,
                      openrs::net::codec::Packet* packet);
};

}  // namespace global

}  // namespace decoder

}  // namespace codec

}  // namespace net

}  // namespace openrs
