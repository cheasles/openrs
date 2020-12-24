#pragma once

#include <openrs/common/io/buffer.h>
#include <openrs/game/player.h>
#include <frozen/map.h>
#include <stdint.h>

#include <map>
#include <memory>

#include "openrs/net/codec/packet.h"

namespace openrs {

namespace net {

namespace codec {

namespace encoder {

namespace global {

class Encoder {
 private:
  static constexpr frozen::map<PacketType, uint8_t, 1> code_mapping_{
      {PacketType::kClientOutdated, 6}};

 public:
  virtual bool Encode(const openrs::net::codec::Packet& packet,
                      const std::weak_ptr<openrs::game::Player>& player,
                      openrs::common::io::Buffer<>* buffer);
};

}  // namespace global

}  // namespace encoder

}  // namespace codec

}  // namespace net

}  // namespace openrs
