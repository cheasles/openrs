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

class AppearanceColumnSet : public ColumnSet {
 public:
  int look_hair;
  int look_beard;
  int look_torso;
  int look_arms;
  int look_forearms;
  int look_legs;
  int look_shoes;
  int colour_shoes;
  int colour_legs;
  int colour_torso;
  int colour_hair;
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
  static inline const std::vector<
      std::tuple<std::string, ColumnSet::member_bind<AppearanceColumnSet>>>
  GetColumnDefinitions() {
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
};

}  // namespace columnsets
}  // namespace database
}  // namespace openrs
