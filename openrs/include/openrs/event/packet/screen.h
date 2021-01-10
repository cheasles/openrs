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

#include <openrs/common/event/event.h>
#include <openrs/game/player.h>

#include <memory>

#include "openrs/net/session.h"

namespace openrs {
namespace event {
namespace packet {

/**
 * An event that describes the screen packet sent for a specific player.
 */
class EventPacketScreen
    : public openrs::common::event::Event<std::shared_ptr<openrs::net::Session>,
                                          openrs::game::Player::DisplayMode,
                                          uint16_t, uint16_t> {
 public:
  EventPacketScreen(std::shared_ptr<openrs::net::Session> session,
                    openrs::game::Player::DisplayMode display_mode,
                    uint16_t screen_width, uint16_t screen_height)
      : openrs::common::event::Event<std::shared_ptr<openrs::net::Session>,
                                     openrs::game::Player::DisplayMode,
                                     uint16_t, uint16_t>(
            session, display_mode, screen_width, screen_height) {}
};

}  // namespace packet
}  // namespace event
}  // namespace openrs
