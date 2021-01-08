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
#include <qtl_mysql.hpp>
#include <qtl_sqlite.hpp>
#include <string>
#include <tuple>
#include <vector>

#include "openrs/database/columnset.h"
#include "openrs/database/columnsets/appearance.h"
#include "openrs/database/columnsets/id.h"
#include "openrs/database/columnsets/player.h"
#include "openrs/database/columnsets/skills.h"
#include "openrs/database/columnsets/worldtile.h"
#include "openrs/database/model.h"

namespace openrs {
namespace database {
namespace models {

struct PlayerModel
    : public openrs::database::Model,
      virtual public openrs::database::columnsets::PlayerColumnSet,
      virtual public openrs::database::columnsets::AppearanceColumnSet,
      virtual public openrs::database::columnsets::SkillsColumnSet,
      virtual public openrs::database::columnsets::WorldTileColumnSet {
  inline static const std::string TABLE_NAME = "players";

  PlayerModel()
      : openrs::database::Model(),
        openrs::database::columnsets::PlayerColumnSet(),
        openrs::database::columnsets::AppearanceColumnSet(),
        openrs::database::columnsets::SkillsColumnSet(),
        openrs::database::columnsets::WorldTileColumnSet() {}

  template <typename Database>
  inline bool Save(Database& database) {
    if (this->id == 0) {
      this->id = database.insert(
          "INSERT INTO " + PlayerModel::TABLE_NAME +
              " (username, password, salt, rights, position_x, position_y) "
              "VALUES (?, ?, ?, ?, ?, ?);",
          std::forward_as_tuple(this->username, this->password, this->salt,
                                this->rights, this->position_x,
                                this->position_y));
    } else {
      database.execute_direct(
          std::string("UPDATE " + PlayerModel::TABLE_NAME +
                      " SET "
                      "username=?, password=?, salt=?, rights=?, "
                      "position_x=?, position_y=? WHERE id=?")
              .c_str(),
          nullptr, this->username, this->password, this->salt, this->rights,
          this->position_x, this->position_y, this->id);
    }

    return true;
  }

  template <typename Database>
  static void CreateModelTable(Database& database);
};

template <>
inline void PlayerModel::CreateModelTable<qtl::mysql::database>(
    qtl::mysql::database& database) {
  throw std::logic_error("Not implemented.");
}

template <>
inline void PlayerModel::CreateModelTable<qtl::sqlite::database>(
    qtl::sqlite::database& database) {
  static const std::string kQuery = "CREATE TABLE IF NOT EXISTS " +
                                    PlayerModel::TABLE_NAME +
                                    " ( "
                                    "id INTEGER PRIMARY KEY, "
                                    "username TEXT NOT NULL UNIQUE, "
                                    "password TEXT NOT NULL, "
                                    "salt TEXT NOT NULL , "
                                    "rights INTEGER NOT NULL DEFAULT 0, "
                                    "position_x INTEGER NOT NULL DEFAULT 0, "
                                    "position_y INTEGER NOT NULL DEFAULT 0 );";
  database.simple_execute(kQuery.c_str());
}

}  // namespace models
}  // namespace database
}  // namespace openrs

namespace qtl {

template <>
inline void
bind_record<qtl::mysql::statement, openrs::database::models::PlayerModel>(
    qtl::mysql::statement& command, openrs::database::models::PlayerModel&& v) {
  bind_record<qtl::mysql::statement, openrs::database::Model>(command,
                                                              std::move(v));
  openrs::database::ColumnSet::BindFields<
      qtl::mysql::statement, openrs::database::columnsets::PlayerColumnSet>(
      command, v);
  openrs::database::ColumnSet::BindFields<
      qtl::mysql::statement, openrs::database::columnsets::AppearanceColumnSet>(
      command, v);
  openrs::database::ColumnSet::BindFields<
      qtl::mysql::statement, openrs::database::columnsets::SkillsColumnSet>(
      command, v);
  openrs::database::ColumnSet::BindFields<
      qtl::mysql::statement, openrs::database::columnsets::WorldTileColumnSet>(
      command, v);
}

template <>
inline void
bind_record<qtl::sqlite::statement, openrs::database::models::PlayerModel>(
    qtl::sqlite::statement& command,
    openrs::database::models::PlayerModel&& v) {
  bind_record<qtl::sqlite::statement, openrs::database::Model>(command,
                                                               std::move(v));
  openrs::database::ColumnSet::BindFields<
      qtl::sqlite::statement, openrs::database::columnsets::PlayerColumnSet>(
      command, v);
  openrs::database::ColumnSet::BindFields<
      qtl::sqlite::statement,
      openrs::database::columnsets::AppearanceColumnSet>(command, v);
  openrs::database::ColumnSet::BindFields<
      qtl::sqlite::statement, openrs::database::columnsets::SkillsColumnSet>(
      command, v);
  openrs::database::ColumnSet::BindFields<
      qtl::sqlite::statement, openrs::database::columnsets::WorldTileColumnSet>(
      command, v);
}

}  // namespace qtl
