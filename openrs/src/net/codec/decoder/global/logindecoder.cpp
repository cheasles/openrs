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
#include "openrs/net/codec/decoder/global/logindecoder.h"

#include <openrs/common/log.h>

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::decoder::global::LoginDecoder::Decode(
    openrs::common::io::Buffer<>& buffer, openrs::net::codec::Packet* packet) {
  if (!packet) {
    return false;
  }

  uint8_t* packet_type_ptr = nullptr;
  if (!buffer.GetData(&packet_type_ptr)) {
    return false;
  }

  const auto kPacketType = LoginDecoder::code_mapping_.find(*packet_type_ptr);
  if (LoginDecoder::code_mapping_.cend() == kPacketType) {
    buffer.seek(SEEK_CUR, -1 * sizeof(uint8_t));
    return Decoder::Decode(buffer, packet);
  }

  packet->type = kPacketType->second;
  if (buffer.remaining() >= 1) {
    packet->data.assign(buffer.cbegin() + buffer.position(), buffer.cend());
    buffer.seek(SEEK_CUR, packet->data.size());
  }

  common::Log(common::Log::LogLevel::kDebug)
      << "[Login] Decoded packet of type "
      << std::to_string(static_cast<uint8_t>(kPacketType->second));

  return true;
}
