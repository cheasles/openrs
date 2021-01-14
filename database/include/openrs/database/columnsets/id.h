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
 * The basic columnset containing an ID column.
 */
class IDColumnSet : public ColumnSet {
 public:
  /**
   * The database row ID that the model inheriting this class relates to.
   */
  int id;

 public:
  IDColumnSet() : id(0) {}

 public:
  /**
   * Get a list of the column names and their associated member variables.
   *
   * @return a vector of tuples containing the column name and the member
   *  variable for binding.
   */
  static inline const std::vector<
      std::tuple<std::string, ColumnSet::member_bind<IDColumnSet>>>
  GetColumnBinds() {
    return {{"id", std::bind(&IDColumnSet::id, std::placeholders::_1)}};
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
