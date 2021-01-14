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

  template <typename Model>
  inline bool GetModel(std::vector<Model>* output) {
    const auto& database_config =
        (*openrs::manager::ConfigManager::get())["database"];
    if (database_config["mode"].get<std::string>() == "mysql") {
      for (auto& record : this->db_mysql->result<Model>("select * from " +
                                                        Model::TABLE_NAME)) {
        output->emplace_back(record);
      }
    } else if (database_config["mode"].get<std::string>() == "sqlite") {
      this->db_sqlite->query(
          "select * from " + Model::TABLE_NAME,
          [&output](const Model& record) { output->emplace_back(record); });
    }
    return output->size() != 0;
  }

  template <typename Field, typename Model>
  inline bool GetModel(const std::string& kWhereField, const Field& kWhereValue,
                       std::vector<Model>* output) const {
    const auto& database_config =
        (*openrs::manager::ConfigManager::get())["database"];
    const std::string kQuery = std::string("select * from ") +
                               Model::TABLE_NAME + " WHERE " + kWhereField +
                               "=?;";
    if (database_config["mode"].get<std::string>() == "mysql") {
      for (auto& record : this->db_mysql->result<Model>(kQuery, kWhereValue)) {
        output->emplace_back(record);
      }
    } else if (database_config["mode"].get<std::string>() == "sqlite") {
      this->db_sqlite->query(
          kQuery, kWhereValue,
          [&output](const Model& record) { output->emplace_back(record); });
    }
    return output->size() != 0;
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
