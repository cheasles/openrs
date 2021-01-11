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

#include "openrs/net/codec/decoder/global/decoder.h"
#include "openrs/net/codec/packet.h"

namespace openrs {
namespace net {
namespace codec {
namespace decoder {
namespace global {

class WorldDecoder : public Decoder,
                     public openrs::common::Singleton<WorldDecoder> {
 private:
  static inline constexpr frozen::map<uint8_t, PacketType, 15> code_mapping_{
      {5, PacketType::kActionButton3},
      {6, PacketType::kActionButton7},
      {14, PacketType::kActionButton1},
      {16, PacketType::kAFK},
      {21, PacketType::kPing},
      {27, PacketType::kActionButton9},
      {30, PacketType::kFinishedLoadingRegion},
      {32, PacketType::kActionButton8},
      {33, PacketType::kReceivedPacketCount},
      {39, PacketType::kUnknown},
      {55, PacketType::kActionButton4},
      {67, PacketType::kActionButton2},
      {68, PacketType::kActionButton5},
      {90, PacketType::kActionButton6},
      {98, PacketType::kScreen},
  };
  static inline constexpr frozen::map<PacketType, int8_t, 15> packet_sizes_{
      {PacketType::kActionButton3, 8},
      {PacketType::kActionButton7, 8},
      {PacketType::kActionButton1, 8},
      {PacketType::kAFK, 2},
      {PacketType::kPing, 0},
      {PacketType::kActionButton9, 8},
      {PacketType::kFinishedLoadingRegion, 0},
      {PacketType::kActionButton8, 8},
      {PacketType::kReceivedPacketCount, 4},
      {PacketType::kUnknown, 4},
      {PacketType::kActionButton4, 8},
      {PacketType::kActionButton2, 8},
      {PacketType::kActionButton5, 8},
      {PacketType::kActionButton6, 8},
      {PacketType::kScreen, 6},
  };

 public:
  bool Decode(openrs::common::io::Buffer<>& buffer,
              openrs::net::codec::Packet* packet) const override;
};

}  // namespace global
}  // namespace decoder
}  // namespace codec
}  // namespace net
}  // namespace openrs
