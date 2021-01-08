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
#include "openrs/net/codec/encoder/global/loginencoder.h"

#include <openrs/common/log.h>

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::encoder::global::LoginEncoder::Encode(
    const openrs::net::codec::Packet& packet,
    const std::weak_ptr<openrs::game::Player>& player,
    openrs::common::io::Buffer<>* buffer) {
  if (!buffer) {
    return false;
  }

  const auto kPacketCode = LoginEncoder::code_mapping_.find(packet.type);
  if (LoginEncoder::code_mapping_.cend() == kPacketCode) {
    return Encoder::Encode(packet, player, buffer);
  }

  if (static_cast<uint8_t>(PacketOpCode::kNone) != kPacketCode->second) {
    buffer->emplace_back(kPacketCode->second);
  }
  if (packet.data.size() != 0) {
    buffer->insert(buffer->cend(), packet.data.cbegin(), packet.data.cend());
  }

  common::Log(common::Log::LogLevel::kDebug)
      << "[Login] Encoded packet of type "
      << std::to_string(kPacketCode->second);

  return true;
}
