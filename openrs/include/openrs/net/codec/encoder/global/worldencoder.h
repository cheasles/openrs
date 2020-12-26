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

class WorldEncoder : public Encoder {
 private:
  enum struct PacketHeaderType : uint8_t { kNone, kUint8, kUint16 };

  static inline constexpr frozen::map<PacketType, uint8_t, 3> code_mapping_{
      {PacketType::kLoginDetails, 2},
      {PacketType::kInterfaceTab, 14},
      {PacketType::kInterfaceWindowPane, 39}};
  static inline constexpr frozen::map<PacketType, PacketHeaderType, 3>
      type_mapping_{
          {PacketType::kLoginDetails, PacketHeaderType::kUint8},
          {PacketType::kInterfaceTab, PacketHeaderType::kNone},
          {PacketType::kInterfaceWindowPane, PacketHeaderType::kNone}};

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
