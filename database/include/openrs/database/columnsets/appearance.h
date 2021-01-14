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

#include <functional>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "openrs/database/columnset.h"

namespace openrs {
namespace database {
namespace columnsets {

/**
 * A set of columns related to a player's appearance.
 */
class AppearanceColumnSet : public ColumnSet {
 public:
  /**
   * Describes the player's hairstyle.
   */
  int look_hair;

  /**
   * Describes the style of the player's the beard.
   */
  int look_beard;

  /**
   * Describes the style of the player's torso.
   */
  int look_torso;

  /**
   * Describes the style of the player's arms.
   */
  int look_arms;

  /**
   * Describes the style of the player's hands/forearms.
   */
  int look_forearms;

  /**
   * Describes the style of the player's legs.
   */
  int look_legs;

  /**
   * Describes the style of the player's shoes.
   */
  int look_shoes;

  /**
   * Describes the colour of the player's shoes.
   */
  int colour_shoes;

  /**
   * Describes the colour of the player's legs.
   */
  int colour_legs;

  /**
   * Describes the colour of the player's torso.
   */
  int colour_torso;

  /**
   * Describes the colour of the player's hair.
   */
  int colour_hair;

  /**
   * Describes the colour of the player's skin.
   */
  int colour_skin;

 public:
  AppearanceColumnSet()
      : look_hair(0),
        look_beard(0),
        look_torso(0),
        look_arms(0),
        look_forearms(0),
        look_legs(0),
        look_shoes(0),
        colour_shoes(0),
        colour_legs(0),
        colour_torso(0),
        colour_hair(0),
        colour_skin(0) {}
  AppearanceColumnSet(const AppearanceColumnSet& other)
      : ColumnSet(other),
        look_hair(other.look_hair),
        look_beard(other.look_beard),
        look_torso(other.look_torso),
        look_arms(other.look_arms),
        look_forearms(other.look_forearms),
        look_legs(other.look_legs),
        look_shoes(other.look_shoes),
        colour_shoes(other.colour_shoes),
        colour_legs(other.colour_legs),
        colour_torso(other.colour_torso),
        colour_hair(other.colour_hair),
        colour_skin(other.colour_skin) {}

 public:
  /**
   * Get a list of the column names and their associated member variables.
   *
   * @return a vector of tuples containing the column name and the member
   *  variable for binding.
   */
  static inline const std::vector<
      std::tuple<std::string, ColumnSet::member_bind<AppearanceColumnSet>>>
  GetColumnBinds() {
    return {
        {"look_hair",
         std::bind(&AppearanceColumnSet::look_hair, std::placeholders::_1)},
        {"look_beard",
         std::bind(&AppearanceColumnSet::look_beard, std::placeholders::_1)},
        {"look_torso",
         std::bind(&AppearanceColumnSet::look_torso, std::placeholders::_1)},
        {"look_arms",
         std::bind(&AppearanceColumnSet::look_arms, std::placeholders::_1)},
        {"look_forearms",
         std::bind(&AppearanceColumnSet::look_forearms, std::placeholders::_1)},
        {"look_legs",
         std::bind(&AppearanceColumnSet::look_legs, std::placeholders::_1)},
        {"look_shoes",
         std::bind(&AppearanceColumnSet::look_shoes, std::placeholders::_1)},
        {"colour_shoes",
         std::bind(&AppearanceColumnSet::colour_shoes, std::placeholders::_1)},
        {"colour_legs",
         std::bind(&AppearanceColumnSet::colour_legs, std::placeholders::_1)},
        {"colour_torso",
         std::bind(&AppearanceColumnSet::colour_torso, std::placeholders::_1)},
        {"colour_hair",
         std::bind(&AppearanceColumnSet::colour_hair, std::placeholders::_1)},
        {"colour_skin",
         std::bind(&AppearanceColumnSet::colour_skin, std::placeholders::_1)},
    };
  }

  /**
   * Get a list of the column names and their associated database properties.
   *
   * @tparam Database The database object to work with. Used for
   *  specialization.
   * @return a vector of tuples containing the column name and the column
   *  description depending on the type of database specified.
   */
  template <typename Database>
  static const std::vector<std::tuple<std::string, std::string>>
  GetColumnDefinitions();
};

}  // namespace columnsets
}  // namespace database
}  // namespace openrs
