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
#include "openrs/game/entity.h"

#include <algorithm>

void openrs::game::Entity::SetNextGraphics(
    const openrs::game::Graphics& kGraphics) {
  if (kGraphics == 0) {
    if (this->graphics_[3] != 0) {
      this->graphics_[3] = kGraphics;
    } else if (this->graphics_[2] != 0) {
      this->graphics_[2] = kGraphics;
    } else if (this->graphics_[1] != 0) {
      this->graphics_[1] = kGraphics;
    } else {
      this->graphics_[0] = kGraphics;
    }
  } else {
    if (std::find(this->graphics_.cbegin(), this->graphics_.cend(),
                  kGraphics) != this->graphics_.cend()) {
      return;
    }
    if (this->graphics_[0] == 0) {
      this->graphics_[0] = kGraphics;
    } else if (this->graphics_[1] == 0) {
      this->graphics_[1] = kGraphics;
    } else if (this->graphics_[2] == 0) {
      this->graphics_[2] = kGraphics;
    } else {
      this->graphics_[3] = kGraphics;
    }
  }
}
