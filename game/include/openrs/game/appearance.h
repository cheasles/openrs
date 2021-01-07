#pragma once

#include <inttypes.h>
#include <openrs/database/columnsets/appearance.h>

#include <cmath>
#include <map>
#include <set>

namespace openrs {
namespace game {

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

}  // namespace game
}  // namespace openrs
