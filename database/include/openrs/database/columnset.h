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

class ColumnSet {
 public:
  template <typename M = ColumnSet>
  using member_bind_int = std::_Bind<int M::*(std::_Placeholder<1>)>;
  template <typename M = ColumnSet>
  using member_bind_string = std::_Bind<std::string M::*(std::_Placeholder<1>)>;

  template <typename M = ColumnSet>
  using member_bind = std::variant<member_bind_int<M>, member_bind_string<M>>;

 public:
  static inline const std::vector<std::tuple<std::string, member_bind<>>>
  GetColumnBinds() {
    return {};
  }

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
