#pragma once

#include <openrs/common/io/buffer.h>
#include <frozen/map.h>
#include <stdint.h>

#include <map>

#include "openrs/net/codec/decoder/global/decoder.h"
#include "openrs/net/codec/packet.h"

namespace openrs {
namespace net {
namespace codec {
namespace decoder {
namespace global {

class WorldDecoder : public Decoder {
 private:
  static inline constexpr frozen::map<uint8_t, PacketType, 3> code_mapping_{
      {16, PacketType::kAFK},
      {21, PacketType::kPing},
      {98, PacketType::kScreen},
  };
  static inline constexpr frozen::map<PacketType, int8_t, 3> packet_sizes_{
      {PacketType::kAFK, 2},
      {PacketType::kPing, 0},
      {PacketType::kScreen, 6},
  };

 public:
  bool Decode(openrs::common::io::Buffer<>& buffer,
              openrs::net::codec::Packet* packet) override;
};

}  // namespace global
}  // namespace decoder
}  // namespace codec
}  // namespace net
}  // namespace openrs
