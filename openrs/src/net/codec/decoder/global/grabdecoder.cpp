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
#include "openrs/net/codec/decoder/global/grabdecoder.h"

#include <iostream>

#include "openrs/net/codec/packet.h"

bool openrs::net::codec::decoder::global::GrabDecoder::Decode(
    openrs::common::io::Buffer<>& buffer, openrs::net::codec::Packet* packet) {
  if (!packet) {
    return false;
  }

  if (buffer.remaining() < 6) {
    return false;
  }

  // Validate the priority.
  if (buffer.at(buffer.position()) > 7) {
    return false;
  }

  packet->type = PacketType::kGrabCache;
  packet->data.assign(buffer.cbegin(), buffer.cend());
  buffer.seek(SEEK_CUR, packet->data.size());

  return true;
}
