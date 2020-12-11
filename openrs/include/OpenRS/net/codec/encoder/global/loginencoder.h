#pragma once

#include <frozen/map.h>
#include <stdint.h>

#include <map>

#include "OpenRS/net/codec/encoder/global/encoder.h"
#include "OpenRS/net/codec/packet.h"
#include "OpenRS/net/io/buffer.h"

namespace openrs {

namespace net {

namespace codec {

namespace encoder {

namespace global {

class LoginEncoder : public Encoder {
 private:
  static constexpr frozen::map<PacketType, uint8_t, 2> code_mapping_{
      {PacketType::kStartUp, 0},
      {PacketType::kGrabCache, static_cast<uint8_t>(PacketOpCode::kNone)}};

 public:
  bool Encode(const openrs::net::codec::Packet& packet,
              openrs::net::io::Buffer* buffer) override;
};

}  // namespace global

}  // namespace encoder

}  // namespace codec

}  // namespace net

}  // namespace openrs
