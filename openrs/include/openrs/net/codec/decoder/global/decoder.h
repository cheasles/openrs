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

#include <openrs/common/io/buffer.h>
#include <frozen/map.h>
#include <stdint.h>

#include <map>

#include "openrs/net/codec/packet.h"

namespace openrs {

namespace net {

namespace codec {

namespace decoder {

namespace global {

class Decoder {
 private:
  static inline constexpr frozen::map<uint8_t, PacketType, 3> code_mapping_{
      {15, PacketType::kHandshake},
      {14, PacketType::kLogin},
      {6, PacketType::kGrabCache},
  };

 public:
  virtual bool Decode(openrs::common::io::Buffer<>& buffer,
                      openrs::net::codec::Packet* packet);
};

}  // namespace global

}  // namespace decoder

}  // namespace codec

}  // namespace net

}  // namespace openrs
