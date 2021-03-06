/**
 * This file is part of OpenRS.
 *
 * OpenRS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * OpenRS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with OpenRS. If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <frozen/map.h>
#include <openrs/common/io/buffer.h>
#include <openrs/common/singleton.h>
#include <stdint.h>

#include <map>

#include "openrs/net/codec/encoder/global/encoder.h"
#include "openrs/net/codec/packet.h"

namespace openrs {
namespace net {
namespace codec {
namespace encoder {
namespace global {

class WorldEncoder : public Encoder,
                     public openrs::common::Singleton<WorldEncoder> {
 private:
  enum struct PacketHeaderType : uint8_t { kNone, kUint8, kUint16 };

  static inline constexpr frozen::map<PacketType, uint8_t, 28> code_mapping_{
      {PacketType::kLoginDetails, 2},
      {PacketType::kMusicEffect, 9},
      {PacketType::kInterfaceTab, 14},
      {PacketType::kRunEnergy, 25},
      {PacketType::kInterfaceWindowPane, 39},
      {PacketType::kInterfaceComponentSetting, 40},
      {PacketType::kMapRegion, 42},
      {PacketType::kConfig2, 56},
      {PacketType::kConfigGlobal2, 63},
      {PacketType::kConfigGameBarPrivate, 75},
      {PacketType::kConfigFile2, 81},
      {PacketType::kConfigGameBarOther, 89},
      {PacketType::kLocalPlayerUpdate, 96},
      {PacketType::kInterfaceComponentAnimation, 103},
      {PacketType::kConfig1, 110},
      {PacketType::kConfigFile1, 111},
      {PacketType::kInterfaceHide, 112},
      {PacketType::kPlayerOption, 118},
      {PacketType::kMusic, 129},
      {PacketType::kInterfaceComponentText, 135},
      {PacketType::kMessage, 136},
      {PacketType::kDynamicMapRegion, 144},
      {PacketType::kPlayerSkill, 146},
      {PacketType::kPing, 153},
      {PacketType::kConfigGlobal1, 154},
      {PacketType::kCreateTile, 158},
      {PacketType::kItemLook, 159},
      {PacketType::kCustomPacket161, 161}};
  static inline constexpr frozen::map<PacketType, PacketHeaderType, 28>
      type_mapping_{
          {PacketType::kLoginDetails, PacketHeaderType::kUint8},
          {PacketType::kMusicEffect, PacketHeaderType::kNone},
          {PacketType::kInterfaceTab, PacketHeaderType::kNone},
          {PacketType::kRunEnergy, PacketHeaderType::kNone},
          {PacketType::kInterfaceWindowPane, PacketHeaderType::kNone},
          {PacketType::kInterfaceComponentSetting, PacketHeaderType::kNone},
          {PacketType::kMapRegion, PacketHeaderType::kUint16},
          {PacketType::kConfig2, PacketHeaderType::kNone},
          {PacketType::kConfigGlobal2, PacketHeaderType::kNone},
          {PacketType::kConfigGameBarPrivate, PacketHeaderType::kNone},
          {PacketType::kConfigFile2, PacketHeaderType::kNone},
          {PacketType::kConfigGameBarOther, PacketHeaderType::kNone},
          {PacketType::kLocalPlayerUpdate, PacketHeaderType::kUint16},
          {PacketType::kInterfaceComponentAnimation, PacketHeaderType::kNone},
          {PacketType::kConfig1, PacketHeaderType::kNone},
          {PacketType::kConfigFile1, PacketHeaderType::kNone},
          {PacketType::kInterfaceHide, PacketHeaderType::kNone},
          {PacketType::kPlayerOption, PacketHeaderType::kUint8},
          {PacketType::kMusic, PacketHeaderType::kNone},
          {PacketType::kInterfaceComponentText, PacketHeaderType::kUint16},
          {PacketType::kMessage, PacketHeaderType::kUint8},
          {PacketType::kDynamicMapRegion, PacketHeaderType::kUint16},
          {PacketType::kPlayerSkill, PacketHeaderType::kNone},
          {PacketType::kConfigGlobal1, PacketHeaderType::kNone},
          {PacketType::kPing, PacketHeaderType::kNone},
          {PacketType::kCreateTile, PacketHeaderType::kNone},
          {PacketType::kItemLook, PacketHeaderType::kNone},
          {PacketType::kCustomPacket161, PacketHeaderType::kNone}};

 public:
  bool Encode(const openrs::net::codec::Packet& packet,
              openrs::common::io::Buffer<>* buffer) const override;
};

}  // namespace global
}  // namespace encoder
}  // namespace codec
}  // namespace net
}  // namespace openrs
