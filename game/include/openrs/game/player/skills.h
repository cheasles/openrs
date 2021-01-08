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
#include <openrs/database/columnsets/skills.h>

#include <cmath>
#include <map>
#include <set>

namespace openrs {
namespace game {
namespace player {

/**
 * Manages the types of skills that are available.
 */
class Skills : virtual public openrs::database::columnsets::SkillsColumnSet {
 public:
  /**
   * The types of skills available to the player.
   */
  enum struct Skill : uint8_t {
    kAttack = 0,
    kDefence = 1,
    kStrength = 2,
    kHitPoints = 3,
    kRange = 4,
    kPrayer = 5,
    kMagic = 6,
    kCooking = 7,
    kWoodCuting = 8,
    kFletching = 9,
    kFishing = 10,
    kFIreMaking = 11,
    kCrafting = 12,
    kSmithing = 13,
    kMining = 14,
    kHerbLore = 15,
    kAgility = 16,
    kThieving = 17,
    kSlayer = 18,
    kFarming = 19,
    kRuneCrafting = 20,
    kConstruction = 22,
    kHunter = 21,
    kSummoning = 23,
    kDungeoneering = 24,
    kLast
  };

 public : Skills() : openrs::database::columnsets::SkillsColumnSet() {}
  /**
   * Gets the current XP count for a specific skill.
   *
   * @param kSkill The specific skill.
   * @return The amount of XP tracked.
   */
  inline uint32_t GetSkillExperience(const Skill& kSkill) const {
    switch (kSkill) {
      case Skill::kAttack:
        return static_cast<uint32_t>(this->skill_attack);
      default:
        return 0;
    }
  }

  /**
   * Gets the current level for a specific skill.
   *
   * @param kSkill The specific skill.
   * @return The current level of the skill.
   */
  inline uint8_t GetSkillLevel(const Skill& kSkill) const {
    uint32_t points = 0;
    uint32_t output = 0;
    const auto& kExperience = this->GetSkillExperience(kSkill);
    for (uint8_t level = 1; level < 99; ++level) {
      points += ::floor(level + 300.0 * ::pow(2.0, level / 7.0));
      output = static_cast<uint32_t>(points / 4);
      if (output - 1 >= kExperience) {
        return level;
      }
    }

    return 99;
  }

  /**
   * Gets the current level combat.
   *
   * @return The current combat level.
   */
  inline uint8_t GetCombatLevel() const { return 99; }
};

}  // namespace player
}  // namespace game
}  // namespace openrs
