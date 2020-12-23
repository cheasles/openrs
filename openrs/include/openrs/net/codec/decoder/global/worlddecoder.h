#pragma once

#include <common/io/buffer.h>
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
  static constexpr frozen::map<uint8_t, PacketType, 3> code_mapping_{
      {16, PacketType::kLoginWorld},
      {18, PacketType::kLoginWorld},
      {19, PacketType::kLoginLobby},
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
