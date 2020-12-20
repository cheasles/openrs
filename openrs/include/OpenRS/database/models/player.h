#pragma once

#include <endian.h>

#include <fstream>
#include <qtl_common.hpp>
#include <qtl_mysql.hpp>
#include <qtl_sqlite.hpp>
#include <string>
#include <vector>

#include "OpenRS/database/model.h"

namespace openrs {
namespace database {
namespace models {

struct PlayerModel : public openrs::database::Model {
  std::string username;
  std::string password;
  std::string salt;

  inline static const std::string TABLE_NAME = "players";

  template <typename Database>
  inline bool Save(Database& database) {
    if (this->id == 0) {
      this->id = database.insert(
          "INSERT INTO " + PlayerModel::TABLE_NAME +
              " (username, password, salt) VALUES (?, ?, ?);",
          std::forward_as_tuple(this->username, this->password, this->salt));
    } else {
      database.execute_direct(
          std::string("UPDATE " + PlayerModel::TABLE_NAME +
                      " SET "
                      " username=?, password=?, salt=? WHERE id=?")
              .c_str(),
          nullptr, this->username, this->password, this->salt, this->id);
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
                                    "salt TEXT NOT NULL );";
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
  qtl::bind_fields(command, v.id, v.username, v.password, v.salt);
}

template <>
inline void
bind_record<qtl::sqlite::statement, openrs::database::models::PlayerModel>(
    qtl::sqlite::statement& command,
    openrs::database::models::PlayerModel&& v) {
  qtl::bind_fields(command, v.id, v.username, v.password, v.salt);
}

}  // namespace qtl
