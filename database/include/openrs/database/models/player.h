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
 public:
  inline static const std::string TABLE_NAME = "players";

 public:
  PlayerModel()
      : openrs::database::Model(),
        openrs::database::columnsets::PlayerColumnSet(),
        openrs::database::columnsets::AppearanceColumnSet(),
        openrs::database::columnsets::SkillsColumnSet(),
        openrs::database::columnsets::WorldTileColumnSet() {}
  inline PlayerModel& operator=(const PlayerModel& kOther) {
    openrs::database::Model::operator=(kOther);
    openrs::database::columnsets::PlayerColumnSet::operator=(kOther);
    openrs::database::columnsets::AppearanceColumnSet::operator=(kOther);
    openrs::database::columnsets::SkillsColumnSet::operator=(kOther);
    openrs::database::columnsets::WorldTileColumnSet::operator=(kOther);
    return *this;
  }

  template <typename Database, typename Statement>
  inline bool Save(Database& database) {
    if (this->id == 0) {
      return Model::Insert<PlayerModel, Database, Statement,
                           openrs::database::columnsets::PlayerColumnSet,
                           openrs::database::columnsets::AppearanceColumnSet,
                           openrs::database::columnsets::SkillsColumnSet,
                           openrs::database::columnsets::WorldTileColumnSet>(
          *this, database, PlayerModel::TABLE_NAME);
    } else {
      return Model::Update<PlayerModel, Database, Statement,
                           openrs::database::columnsets::PlayerColumnSet,
                           openrs::database::columnsets::AppearanceColumnSet,
                           openrs::database::columnsets::SkillsColumnSet,
                           openrs::database::columnsets::WorldTileColumnSet>(
          *this, database, PlayerModel::TABLE_NAME);
    }
  }

  template <typename Database>
  inline static bool CreateModelTable(Database& database) {
    return Model::CreateTable<Database,
                              openrs::database::columnsets::PlayerColumnSet,
                              openrs::database::columnsets::AppearanceColumnSet,
                              openrs::database::columnsets::SkillsColumnSet,
                              openrs::database::columnsets::WorldTileColumnSet>(
        database, PlayerModel::TABLE_NAME);
  }

  /**
   * Selects a set of rows inside a table in the database.
   *
   * @tparam Database The type of database to use.
   * @param database The database object itself.
   * @param output The location to store the results.
   * @return True on success, false otherwise.
   */
  template <typename Database>
  inline static bool Select(Database& database,
                            std::vector<PlayerModel>* output) {
    return Model::Select<PlayerModel, Database,
                         openrs::database::columnsets::PlayerColumnSet,
                         openrs::database::columnsets::AppearanceColumnSet,
                         openrs::database::columnsets::SkillsColumnSet,
                         openrs::database::columnsets::WorldTileColumnSet>(
        database, PlayerModel::TABLE_NAME, output);
  }

  /**
   * Selects a set of rows inside a table in the database.
   *
   * @tparam Database The type of database to use.
   * @param database The database object itself.
   * @param kWhereField The name of the column to use in the WHERE clause.
   * @param kWhereValue The value to search for.
   * @param output The location to store the results.
   * @return True on success, false otherwise.
   */
  template <typename Database, typename Field>
  inline static bool Select(Database& database, const std::string& kWhereField,
                            const Field& kWhereValue,
                            std::vector<PlayerModel>* output) {
    return Model::Select<PlayerModel, Database, Field,
                         openrs::database::columnsets::PlayerColumnSet,
                         openrs::database::columnsets::AppearanceColumnSet,
                         openrs::database::columnsets::SkillsColumnSet,
                         openrs::database::columnsets::WorldTileColumnSet>(
        database, PlayerModel::TABLE_NAME, kWhereField, kWhereValue, output);
  }
};

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
