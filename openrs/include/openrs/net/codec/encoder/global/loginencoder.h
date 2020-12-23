#pragma once

#include <common/io/buffer.h>
#include <frozen/map.h>
#include <stdint.h>

#include <map>

#include "openrs/net/codec/encoder/global/encoder.h"
#include "openrs/net/codec/packet.h"

namespace openrs {

namespace net {

namespace codec {

namespace encoder {

namespace global {

class LoginEncoder : public Encoder {
 private:
  static constexpr frozen::map<PacketType, uint8_t, 5> code_mapping_{
      {PacketType::kStartUp, 0},
      {PacketType::kErrorSession, 10},
      {PacketType::kErrorInvalidLogin, 3},
      {PacketType::kErrorInvalidLoginServer, 20},
      {PacketType::kGrabCache, static_cast<uint8_t>(PacketOpCode::kNone)}};

 public:
  bool Encode(const openrs::net::codec::Packet& packet,
              openrs::common::io::Buffer<>* buffer) override;
};

}  // namespace global

}  // namespace encoder

}  // namespace codec

}  // namespace net

}  // namespace openrs
