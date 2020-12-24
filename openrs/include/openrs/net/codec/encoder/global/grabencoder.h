#pragma once

#include <frozen/map.h>
#include <openrs/common/io/buffer.h>
#include <stdint.h>

#include <map>

#include "openrs/net/codec/encoder/global/encoder.h"
#include "openrs/net/codec/packet.h"

namespace openrs {

namespace net {

namespace codec {

namespace encoder {

namespace global {

class GrabEncoder : public Encoder {
 private:
  static inline constexpr frozen::map<PacketType, uint8_t, 2> code_mapping_{
      {PacketType::kStartUp, 0},
      {PacketType::kGrabCache, static_cast<uint8_t>(PacketOpCode::kNone)}};

 public:
  bool Encode(const openrs::net::codec::Packet& packet,
              const std::weak_ptr<openrs::game::Player>& player,
              openrs::common::io::Buffer<>* buffer) override;
};

}  // namespace global

}  // namespace encoder

}  // namespace codec

}  // namespace net

}  // namespace openrs
