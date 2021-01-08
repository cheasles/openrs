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

class SkillsColumnSet : public ColumnSet {
 public:
  int skill_attack;

 public:
  SkillsColumnSet() : skill_attack(0) {}
  SkillsColumnSet(const SkillsColumnSet& other)
      : ColumnSet(other), skill_attack(other.skill_attack) {}

 public:
  static inline const std::vector<
      std::tuple<std::string, ColumnSet::member_bind<SkillsColumnSet>>>
  GetColumnDefinitions() {
    return {
        {"skill_attack",
         std::bind(&SkillsColumnSet::skill_attack, std::placeholders::_1)},
    };
  }
};

}  // namespace columnsets
}  // namespace database
}  // namespace openrs
