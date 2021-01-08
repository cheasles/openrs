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

class EventLogin : public openrs::common::event::Event<
                       std::shared_ptr<openrs::game::Player>,
                       std::shared_ptr<openrs::net::Session>> {
 public:
  EventLogin(std::shared_ptr<openrs::game::Player> player,
             std::shared_ptr<openrs::net::Session> session)
      : openrs::common::event::Event<std::shared_ptr<openrs::game::Player>,
                                     std::shared_ptr<openrs::net::Session>>(
            player, session) {}
};

}  // namespace event
}  // namespace openrs
