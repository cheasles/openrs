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

#include "openrs/game/worldtile.h"

namespace openrs {
namespace game {

class Graphics {
 private:
  uint16_t id_;
  uint32_t height_;
  uint32_t speed_;
  uint32_t rotation_;

 public:
  Graphics(uint16_t id, uint32_t height, uint32_t speed, uint32_t rotation)
      : id_(id), height_(height), speed_(speed), rotation_(rotation) {}
  Graphics(uint16_t id) : Graphics(id, 0, 0, 0) {}
  Graphics() : Graphics(0) {}

  inline bool operator==(const uint16_t& rhs) const { return this->id_ == rhs; }
  inline bool operator!=(const uint16_t& rhs) const { return !(*this == rhs); }

  inline bool operator==(const Graphics& rhs) const {
    return this->id_ == rhs.id_ && this->height_ == rhs.height_ &&
           this->speed_ == rhs.speed_ && this->rotation_ == rhs.rotation_;
  }
  inline bool operator!=(const Graphics& rhs) const { return !(*this != rhs); }

  /**
   * Get a hash of the speed and height for this graphic.
   *
   * @return Hash int.
   */
  inline uint32_t GetSettingsHash() const {
    return (this->speed_ & 0xFFFF) | (this->height_ << 16);
  }

  /**
   * Get a hash of the rotation for this graphic.
   *
   * @return Hash byte.
   */
  inline uint8_t GetSettingsHash2() const { return this->rotation_ & 0x7; }

  inline auto id() const { return this->id_; }
  inline auto height() const { return this->height_; }
  inline auto speed() const { return this->speed_; }
  inline auto rotation() const { return this->rotation_; }
};

}  // namespace game
}  // namespace openrs
