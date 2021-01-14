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
#include "openrs/database/columnsets/appearance.h"

#include <qtl_mysql.hpp>
#include <qtl_sqlite.hpp>

template <>
const std::vector<std::tuple<std::string, std::string>>
openrs::database::columnsets::AppearanceColumnSet::GetColumnDefinitions<
    qtl::mysql::database>() {
  throw std::logic_error("Not implemented.");
}

template <>
const std::vector<std::tuple<std::string, std::string>>
openrs::database::columnsets::AppearanceColumnSet::GetColumnDefinitions<
    qtl::sqlite::database>() {
  return {{"look_hair", "INTEGER NOT NULL DEFAULT 0"},
          {"look_beard", "INTEGER NOT NULL DEFAULT 0"},
          {"look_torso", "INTEGER NOT NULL DEFAULT 0"},
          {"look_arms", "INTEGER NOT NULL DEFAULT 0"},
          {"look_forearms", "INTEGER NOT NULL DEFAULT 0"},
          {"look_legs", "INTEGER NOT NULL DEFAULT 0"},
          {"look_shoes", "INTEGER NOT NULL DEFAULT 0"},
          {"colour_shoes", "INTEGER NOT NULL DEFAULT 0"},
          {"colour_legs", "INTEGER NOT NULL DEFAULT 0"},
          {"colour_torso", "INTEGER NOT NULL DEFAULT 0"},
          {"colour_hair", "INTEGER NOT NULL DEFAULT 0"},
          {"colour_skin", "INTEGER NOT NULL DEFAULT 0"}};
}
