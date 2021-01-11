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
#include "openrs/net/codec/decoder/global/decoder.h"

#include <openrs/common/log.h>

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::decoder::global::Decoder::Decode(
    openrs::common::io::Buffer<>& buffer,
    openrs::net::codec::Packet* packet) const {
  if (!packet) {
    return false;
  }

  uint8_t* packet_type_ptr = nullptr;
  if (!buffer.GetData(&packet_type_ptr)) {
    return false;
  }

  const auto kPacketType = Decoder::code_mapping_.find(*packet_type_ptr);
  if (Decoder::code_mapping_.cend() == kPacketType) {
    common::Log(common::Log::LogLevel::kWarning)
        << "Invalid packet code: " << std::to_string(*packet_type_ptr);
    return false;
  }

  packet->type = kPacketType->second;
  if (buffer.remaining() > 0) {
    uint8_t* packet_size_ptr = nullptr;
    if (!buffer.GetData(&packet_size_ptr)) {
      return false;
    }

    const auto cbegin = buffer.cbegin() + buffer.position();
    const auto cend = cbegin + *packet_size_ptr;

    if (cbegin >= buffer.cend() || cend > buffer.cend()) {
      return false;
    }

    packet->data.assign(cbegin, cend);
    buffer.seek(SEEK_CUR, packet->data.size());
  }

  common::Log(common::Log::LogLevel::kDebug)
      << "Decoded packet of type "
      << std::to_string(static_cast<uint8_t>(kPacketType->second));

  return true;
}
