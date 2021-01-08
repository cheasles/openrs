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

#include <stdint.h>

#include <map>
#include <vector>

#include "openrs/net/session.h"
#include "openrs/net/codec/handler/global/packethandler.h"
#include "openrs/net/codec/packet.h"

namespace openrs {

namespace net {

class Session;

}  // namespace net

}  // namespace openrs

namespace openrs {

namespace net {

namespace codec {

namespace handler {

namespace global {

class LoginPacketHandler : public PacketHandler {
 public:
  void Handle(openrs::net::codec::Packet& packet,
              openrs::net::Session* session) override;
};

}  // namespace global

}  // namespace handler

}  // namespace codec

}  // namespace net

}  // namespace openrs
