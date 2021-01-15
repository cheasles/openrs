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
#include <openrs/game/entity.h>
#include <openrs/game/worldtile.h>

#include <memory>

#include "openrs/net/session.h"

namespace openrs {
namespace event {
namespace entity {

/**
 * An event that details an entity moving.
 * 
 * @note This should be issued at the start of the move.
 */
class EventEntityMove
    : public openrs::common::event::Event<std::shared_ptr<openrs::game::Entity>,
                                          openrs::game::WorldTile> {
 public:
  EventEntityMove(std::shared_ptr<openrs::game::Entity> entity,
                  openrs::game::WorldTile previous_position)
      : openrs::common::event::Event<std::shared_ptr<openrs::game::Entity>,
                                     openrs::game::WorldTile>(
            entity, previous_position) {}
};

}  // namespace entity
}  // namespace event
}  // namespace openrs
