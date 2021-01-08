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
#include <openrs/database/columnsets/appearance.h>

#include <cmath>
#include <map>
#include <set>

namespace openrs {
namespace game {
namespace player {

/**
 * Manages the appearance options of an entity.
 */
class Appearance
    : virtual public openrs::database::columnsets::AppearanceColumnSet {
 public:
  /**
   * Body parts that can be coloured or styled.
   */
  enum struct BodyPart : uint8_t {
    kHair,
    kBeard,
    kTorso,
    kLegs,
    kBoots,
    kSkin,
    kArms,
    kForearms,
  };

 public:
  Appearance() : openrs::database::columnsets::AppearanceColumnSet() {}

  /**
   * Gets the style of a specific body part.
   *
   * @param kPart The specific body part.
   * @return The style of the specified body part.
   */
  inline uint16_t GetAppearanceLook(const BodyPart& kPart) {
    switch (kPart) {
      case BodyPart::kHair:
        return static_cast<uint16_t>(this->look_hair);
      case BodyPart::kBeard:
        return static_cast<uint16_t>(this->look_beard);
      case BodyPart::kTorso:
        return static_cast<uint16_t>(this->look_torso);
      case BodyPart::kArms:
        return static_cast<uint16_t>(this->look_arms);
      case BodyPart::kForearms:
        return static_cast<uint16_t>(this->look_forearms);
      case BodyPart::kLegs:
        return static_cast<uint16_t>(this->look_legs);
      case BodyPart::kBoots:
        return static_cast<uint16_t>(this->look_shoes);
      default:
        return 0;
    }
  }

  /**
   * Gets the colour of a specific body part.
   *
   * @param kPart The specific body part.
   * @return The colour of the specified body part.
   */
  inline uint8_t GetAppearanceColour(const BodyPart& kPart) {
    switch (kPart) {
      case BodyPart::kHair:
        return static_cast<uint8_t>(this->colour_hair);
      case BodyPart::kTorso:
        return static_cast<uint8_t>(this->colour_torso);
      case BodyPart::kLegs:
        return static_cast<uint8_t>(this->colour_legs);
      case BodyPart::kBoots:
        return static_cast<uint8_t>(this->colour_shoes);
      case BodyPart::kSkin:
        return static_cast<uint8_t>(this->colour_shoes);
      default:
        return 0;
    }
  }
};

}  // namespace player
}  // namespace game
}  // namespace openrs
