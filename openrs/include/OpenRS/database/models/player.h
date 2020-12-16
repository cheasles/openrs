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

  template <typename Database>
  static void CreateModelTable(Database& database);

  bool CheckPassword(const std::vector<uint8_t>& kPassword,
                     const uint32_t& kIterations = 10) const;
};

template <>
inline void PlayerModel::CreateModelTable<qtl::mysql::database>(
    qtl::mysql::database& database) {
  throw std::logic_error("Not implemented.");
}

template <>
inline void PlayerModel::CreateModelTable<qtl::sqlite::database>(
    qtl::sqlite::database& database) {
  database.simple_execute(
      "CREATE TABLE IF NOT EXISTS players ( "
      "id INTEGER PRIMARY KEY, "
      "username TEXT NOT NULL UNIQUE, "
      "password TEXT NOT NULL, "
      "salt TEXT NOT NULL );");
}

}  // namespace models
}  // namespace database
}  // namespace openrs

namespace qtl {

template <>
inline void
bind_record<qtl::mysql::statement, openrs::database::models::PlayerModel>(
    qtl::mysql::statement& command, openrs::database::models::PlayerModel&& v) {
  qtl::bind_record<qtl::mysql::statement, openrs::database::Model>(
      command, std::move(v));
  qtl::bind_field(command, 1, v.username);
  qtl::bind_field(command, 2, v.password);
  qtl::bind_field(command, 3, v.salt);
}

template <>
inline void
bind_record<qtl::sqlite::statement, openrs::database::models::PlayerModel>(
    qtl::sqlite::statement& command,
    openrs::database::models::PlayerModel&& v) {
  qtl::bind_record<qtl::sqlite::statement, openrs::database::Model>(
      command, std::move(v));
  qtl::bind_field(command, 1, v.username);
  qtl::bind_field(command, 2, v.password);
  qtl::bind_field(command, 3, v.salt);
}

}  // namespace qtl
