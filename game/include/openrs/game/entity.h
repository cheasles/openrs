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

#include <inttypes.h>

#include <array>

#include "openrs/game/graphics.h"
#include "openrs/game/worldtile.h"

namespace openrs {
namespace game {

class Entity : public openrs::game::WorldTile {
 public:
  enum struct Direction : uint8_t { kNorth, kEast, kSouth, kWest };

 protected:
  Direction direction_;
  std::array<openrs::game::Graphics, 4> graphics_;
  uint32_t face_entity_last_;
  uint32_t face_entity_next_;

 public:
  Entity()
      : direction_(Direction::kNorth),
        face_entity_last_(-1),
        face_entity_next_(-2) {}

  /**
   * Sets or resets the next graphics slot to the input value.
   *
   * @param kGraphics The next graphics value to set. Can be zero.
   */
  void SetNextGraphics(const openrs::game::Graphics& kGraphics);

  inline auto direction() const { return this->direction_; }
  inline const auto& graphics() const { return this->graphics_; }
  inline auto face_entity_last() const { return this->face_entity_last_; }
  inline auto face_entity_next() const { return this->face_entity_next_; }
};

}  // namespace game
}  // namespace openrs
