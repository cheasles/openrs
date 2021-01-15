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
#include <qtl_common.hpp>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

template <class>
inline constexpr bool always_false_v = false;

namespace openrs {
namespace database {

/**
 * Describes a set of columns in a database and their member variables.
 *
 * This is a helper interface class to define a subset of columns within a
 * database table so that they can be managed in distinct groups. For example,
 * a table containing information about players might include their login
 * information, their skill levels and their appearance. To avoid defining one
 * massive list of all columns, we can instead define three distinct ColumnSet
 * classes for each category.
 */
class ColumnSet {
 public:
  /**
   * The type used when binding to an integer member.
   *
   * @tparam M The ColumnSet type the bind refers to.
   */
  template <typename M = ColumnSet>
  using member_bind_int = std::_Bind<int M::*(std::_Placeholder<1>)>;

  /**
   * The type used when binding to a string member.
   *
   * @tparam M The ColumnSet type the bind refers to.
   */
  template <typename M = ColumnSet>
  using member_bind_string = std::_Bind<std::string M::*(std::_Placeholder<1>)>;

  /**
   * Describes all types that can be bound to.
   *
   * @tparam M The ColumnSet type the bind refers to.
   */
  template <typename M = ColumnSet>
  using member_bind = std::variant<member_bind_int<M>, member_bind_string<M>>;

 public:
  /**
   * Retrieves a list of bindings for this set of columns.
   *
   * @return A vector of strings paired with a member variable bind.
   */
  static inline const std::vector<std::tuple<std::string, member_bind<>>>
  GetColumnBinds() {
    return {};
  }

  /**
   * Retrieves a list of bindings for a set of columns.
   *
   * @tparam Statement The database statement type to work with.
   * @tparam ColumnSet The type of ColumnSet to retrieve binds for.
   * @param command The database statement to bind to.
   * @param v An instance of the ColumnSet object to bind with.
   * @return A vector of strings paired with a member variable bind.
   */
  template <typename Statement, typename ColumnSet>
  static constexpr inline void BindFields(Statement& command, ColumnSet& v) {
    auto columns = ColumnSet::GetColumnBinds();
    for (auto& column : columns) {
      std::visit(
          [&v, &column, &command](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T,
                                         openrs::database::ColumnSet::
                                             member_bind_string<ColumnSet>>) {
              qtl::bind_field<Statement, std::string>(
                  command, std::get<0>(column).c_str(), arg(v));
            } else if constexpr (std::is_same_v<
                                     T, openrs::database::ColumnSet::
                                            member_bind_int<ColumnSet>>) {
              qtl::bind_field<Statement, int>(
                  command, std::get<0>(column).c_str(), arg(v));
            } else {
              static_assert(always_false_v<T>,
                            "unknown model column bind type");
            }
          },
          std::get<1>(column));
    }
  }
};

}  // namespace database
}  // namespace openrs
