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

class IDColumnSet : public ColumnSet {
 public:
  int id;

 public:
  IDColumnSet() : id(0) {}

 public:
  static inline const std::vector<
      std::tuple<std::string, ColumnSet::member_bind<IDColumnSet>>>
  GetColumnDefinitions() {
    return {{"id", std::bind(&IDColumnSet::id, std::placeholders::_1)}};
  }
};

}  // namespace columnsets
}  // namespace database
}  // namespace openrs
