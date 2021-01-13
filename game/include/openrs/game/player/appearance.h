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
   * Set the appearance to the default male character.
   */
  inline void SetDefaultMale() {
    this->SetAppearanceLook(BodyPart::kHair, 7);
    this->SetAppearanceLook(BodyPart::kBeard, 14);
    this->SetAppearanceLook(BodyPart::kTorso, 18);
    this->SetAppearanceLook(BodyPart::kArms, 26);
    this->SetAppearanceLook(BodyPart::kForearms, 34);
    this->SetAppearanceLook(BodyPart::kLegs, 38);
    this->SetAppearanceLook(BodyPart::kBoots, 42);
    this->SetAppearanceColour(BodyPart::kBoots, 0);
    this->SetAppearanceColour(BodyPart::kLegs, 32);
    this->SetAppearanceColour(BodyPart::kTorso, 95);
    this->SetAppearanceColour(BodyPart::kHair, 17);
  }

  /**
   * Set the appearance to the default female character.
   */
  inline void SetDefaultFemale() {
    this->SetAppearanceLook(BodyPart::kHair, 276);
    this->SetAppearanceLook(BodyPart::kBeard, 57);
    this->SetAppearanceLook(BodyPart::kTorso, 57);
    this->SetAppearanceLook(BodyPart::kArms, 65);
    this->SetAppearanceLook(BodyPart::kForearms, 68);
    this->SetAppearanceLook(BodyPart::kLegs, 77);
    this->SetAppearanceLook(BodyPart::kBoots, 80);
    this->SetAppearanceColour(BodyPart::kBoots, 0);
    this->SetAppearanceColour(BodyPart::kLegs, 95);
    this->SetAppearanceColour(BodyPart::kTorso, 95);
    this->SetAppearanceColour(BodyPart::kHair, 78);
  }

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

  /**
   * Sets the style of a specific body part.
   *
   * @param kPart The specific body part.
   * @param kStyle The style of the specified body part.
   */
  inline void SetAppearanceLook(const BodyPart& kPart, const uint16_t& kStyle) {
    switch (kPart) {
      case BodyPart::kHair:
        this->look_hair = kStyle;
        break;
      case BodyPart::kBeard:
        this->look_beard = kStyle;
        break;
      case BodyPart::kTorso:
        this->look_torso = kStyle;
        break;
      case BodyPart::kArms:
        this->look_arms = kStyle;
        break;
      case BodyPart::kForearms:
        this->look_forearms = kStyle;
        break;
      case BodyPart::kLegs:
        this->look_legs = kStyle;
        break;
      case BodyPart::kBoots:
        this->look_shoes = kStyle;
        break;
      default:
        throw std::logic_error("Unknown bodypart specified in appearance.");
    }
  }

  /**
   * Sets the colour of a specific body part.
   *
   * @param kPart The specific body part.
   * @param kColour The colour of the specified body part.
   */
  inline void SetAppearanceColour(const BodyPart& kPart,
                                  const uint8_t& kColour) {
    switch (kPart) {
      case BodyPart::kHair:
        this->colour_hair = kColour;
        break;
      case BodyPart::kTorso:
        this->colour_torso = kColour;
        break;
      case BodyPart::kLegs:
        this->colour_legs = kColour;
        break;
      case BodyPart::kBoots:
        this->colour_shoes = kColour;
        break;
      case BodyPart::kSkin:
        this->colour_shoes = kColour;
        break;
      default:
        throw std::logic_error("Unknown bodypart specified in appearance.");
    }
  }
};

}  // namespace player
}  // namespace game
}  // namespace openrs
