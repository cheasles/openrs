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

class WorldTileColumnSet : public ColumnSet {
 public:
  int position_x;
  int position_y;
  int position_z;

 public:
  WorldTileColumnSet() : position_x(0), position_y(0), position_z(0) {}
  WorldTileColumnSet(const WorldTileColumnSet& other)
      : position_x(other.position_x),
        position_y(other.position_y),
        position_z(other.position_z) {}
  WorldTileColumnSet(const int& x, const int& y, const int& z)
      : position_x(x), position_y(y), position_z(z) {}

 public:
  static inline const std::vector<
      std::tuple<std::string, ColumnSet::member_bind<WorldTileColumnSet>>>
  GetColumnDefinitions() {
    return {
        {"position_x",
         std::bind(&WorldTileColumnSet::position_x, std::placeholders::_1)},
        {"position_y",
         std::bind(&WorldTileColumnSet::position_y, std::placeholders::_1)},
        {"position_z",
         std::bind(&WorldTileColumnSet::position_z, std::placeholders::_1)},
    };
  }
};

}  // namespace columnsets
}  // namespace database
}  // namespace openrs
