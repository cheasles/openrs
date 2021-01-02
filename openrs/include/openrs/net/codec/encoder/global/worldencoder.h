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

  static inline constexpr frozen::map<PacketType, uint8_t, 24> code_mapping_{
      {PacketType::kLoginDetails, 2},
      {PacketType::kMusicEffect, 9},
      {PacketType::kInterfaceTab, 14},
      {PacketType::kRunEnergy, 25},
      {PacketType::kInterfaceWindowPane, 39},
      {PacketType::kInterfaceComponentSetting, 40},
      {PacketType::kMapRegion, 42},
      {PacketType::kVariable2, 56},
      {PacketType::kConfigGlobal2, 63},
      {PacketType::kConfigFile2, 81},
      {PacketType::kInterfaceComponentAnimation, 103},
      {PacketType::kVariable1, 110},
      {PacketType::kConfigFile1, 111},
      {PacketType::kInterfaceHide, 112},
      {PacketType::kPlayerOption, 118},
      {PacketType::kMusic, 129},
      {PacketType::kInterfaceComponentText, 135},
      {PacketType::kMessage, 136},
      {PacketType::kDynamicMapRegion, 144},
      {PacketType::kPing, 153},
      {PacketType::kConfigGlobal1, 154},
      {PacketType::kCreateTile, 158},
      {PacketType::kItemLook, 159},
      {PacketType::kCustomPacket161, 161}};
  static inline constexpr frozen::map<PacketType, PacketHeaderType, 24>
      type_mapping_{
          {PacketType::kLoginDetails, PacketHeaderType::kUint8},
          {PacketType::kMusicEffect, PacketHeaderType::kNone},
          {PacketType::kInterfaceTab, PacketHeaderType::kNone},
          {PacketType::kRunEnergy, PacketHeaderType::kNone},
          {PacketType::kInterfaceWindowPane, PacketHeaderType::kNone},
          {PacketType::kInterfaceComponentSetting, PacketHeaderType::kNone},
          {PacketType::kMapRegion, PacketHeaderType::kUint16},
          {PacketType::kVariable2, PacketHeaderType::kNone},
          {PacketType::kConfigFile2, PacketHeaderType::kNone},
          {PacketType::kInterfaceComponentAnimation, PacketHeaderType::kNone},
          {PacketType::kVariable1, PacketHeaderType::kNone},
          {PacketType::kConfigFile1, PacketHeaderType::kNone},
          {PacketType::kInterfaceHide, PacketHeaderType::kNone},
          {PacketType::kPlayerOption, PacketHeaderType::kUint8},
          {PacketType::kMusic, PacketHeaderType::kNone},
          {PacketType::kInterfaceComponentText, PacketHeaderType::kNone},
          {PacketType::kMessage, PacketHeaderType::kUint8},
          {PacketType::kDynamicMapRegion, PacketHeaderType::kUint16},
          {PacketType::kConfigGlobal2, PacketHeaderType::kNone},
          {PacketType::kConfigGlobal1, PacketHeaderType::kNone},
          {PacketType::kPing, PacketHeaderType::kNone},
          {PacketType::kCreateTile, PacketHeaderType::kNone},
          {PacketType::kItemLook, PacketHeaderType::kNone},
          {PacketType::kCustomPacket161, PacketHeaderType::kNone}};

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
