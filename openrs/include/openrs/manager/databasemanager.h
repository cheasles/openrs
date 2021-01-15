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

#include <inttypes.h>
#include <openrs/common/io/buffer.h>
#include <openrs/common/singleton.h>
#include <openrs/database/model.h>

#include <memory>
#include <nlohmann/json.hpp>
#include <qtl_mysql.hpp>
#include <qtl_sqlite.hpp>

#include "openrs/manager/configmanager.h"
#include "openrs/manager/manager.h"

namespace openrs {

namespace manager {

class DatabaseManager : public openrs::manager::Manager,
                        public openrs::common::Singleton<DatabaseManager> {
 private:
  std::shared_ptr<qtl::mysql::database> db_mysql;
  std::shared_ptr<qtl::sqlite::database> db_sqlite;

 public:
  DatabaseManager(void) {}
  virtual ~DatabaseManager();

  bool Init() override;

  /**
   * Selects a set of rows inside a table in the database.
   *
   * @tparam Model The type of model to select from the database.
   * @param output The location to store the results.
   * @return True on success, false otherwise.
   */
  template <typename Model>
  inline bool GetModel(std::vector<Model>* output) {
    const auto& database_config =
        (*openrs::manager::ConfigManager::get())["database"];
    if (database_config["mode"].get<std::string>() == "mysql") {
      return Model::template Select<qtl::mysql::database>(*this->db_mysql.get(),
                                                          output);
    } else if (database_config["mode"].get<std::string>() == "sqlite") {
      return Model::template Select<qtl::sqlite::database>(
          *this->db_sqlite.get(), output);
    }

    return false;
  }

  /**
   * Selects a set of rows inside a table in the database.
   *
   * @tparam Field The type of data stored in the where field.
   * @tparam Model The type of model to select from the database.
   * @param kWhereField The name of the column to use in the WHERE clause.
   * @param kWhereValue The value to search for.
   * @param output The location to store the results.
   * @return True on success, false otherwise.
   */
  template <typename Field, typename Model>
  inline bool GetModel(const std::string& kWhereField, const Field& kWhereValue,
                       std::vector<Model>* output) const {
    const auto& database_config =
        (*openrs::manager::ConfigManager::get())["database"];
    if (database_config["mode"].get<std::string>() == "mysql") {
      return Model::template Select<qtl::mysql::database, Field>(
          *this->db_mysql.get(), kWhereField, kWhereValue, output);
    } else if (database_config["mode"].get<std::string>() == "sqlite") {
      return Model::template Select<qtl::sqlite::database, Field>(
          *this->db_sqlite.get(), kWhereField, kWhereValue, output);
    }

    return false;
  }

  template <typename Model>
  inline bool CreateModel(Model& model) {
    const auto& database_config =
        (*openrs::manager::ConfigManager::get())["database"];
    if (database_config["mode"].get<std::string>() == "mysql") {
      return model.template Save<qtl::mysql::database, qtl::mysql::statement>(
          *this->db_mysql.get());
    } else if (database_config["mode"].get<std::string>() == "sqlite") {
      return model.template Save<qtl::sqlite::database, qtl::sqlite::statement>(
          *this->db_sqlite.get());
    }

    return false;
  }

 private:
  bool InitMySQL();
  bool InitSqlite();
};

}  // namespace manager

}  // namespace openrs
