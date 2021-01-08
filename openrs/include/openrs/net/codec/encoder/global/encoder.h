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
#include <openrs/game/player.h>
#include <frozen/map.h>
#include <stdint.h>

#include <map>
#include <memory>

#include "openrs/net/codec/packet.h"

namespace openrs {

namespace net {

namespace codec {

namespace encoder {

namespace global {

class Encoder {
 private:
  static constexpr frozen::map<PacketType, uint8_t, 1> code_mapping_{
      {PacketType::kClientOutdated, 6}};

 public:
  virtual bool Encode(const openrs::net::codec::Packet& packet,
                      const std::weak_ptr<openrs::game::Player>& player,
                      openrs::common::io::Buffer<>* buffer);
};

}  // namespace global

}  // namespace encoder

}  // namespace codec

}  // namespace net

}  // namespace openrs
