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
#include "openrs/net/codec/encoder/global/worldencoder.h"

#include <openrs/common/log.h>

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::encoder::global::WorldEncoder::Encode(
    const openrs::net::codec::Packet& packet,
    const std::weak_ptr<openrs::game::Player>& kPlayer,
    openrs::common::io::Buffer<>* buffer) {
  if (!buffer) {
    return false;
  }

  const auto kPacketCode = WorldEncoder::code_mapping_.find(packet.type);
  if (WorldEncoder::code_mapping_.cend() == kPacketCode) {
    return Encoder::Encode(packet, kPlayer, buffer);
  }

  const auto kPacketType = WorldEncoder::type_mapping_.find(packet.type);
  if (static_cast<uint8_t>(PacketOpCode::kNone) != kPacketCode->second) {
    if (auto player = kPlayer.lock()) {
      if (kPacketCode->second >= 128) {
        buffer->PutData<uint8_t>(
            static_cast<uint8_t>(kPacketCode->second >> 8) + 128);
        buffer->PutData<uint8_t>(kPacketCode->second);
      } else {
        buffer->PutData<uint8_t>(kPacketCode->second);
      }
    } else {
      buffer->PutSmartBE(kPacketCode->second);
    }
  }
  switch (kPacketType->second) {
    case PacketHeaderType::kUint16:
      buffer->PutDataBE<uint16_t>(packet.data.size());
      break;
    case PacketHeaderType::kUint8:
      buffer->PutDataBE<uint8_t>(packet.data.size());
      break;
    default:
      break;
  }

  if (packet.data.size() != 0) {
    buffer->insert(buffer->cend(), packet.data.cbegin(), packet.data.cend());
  }

  common::Log(common::Log::LogLevel::kDebug)
      << "[World] Encoded packet of type "
      << std::to_string(kPacketCode->second);

  return true;
}
