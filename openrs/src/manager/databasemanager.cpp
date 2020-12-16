#include "OpenRS/manager/databasemanager.h"

#include <iomanip>
#include <nlohmann/json.hpp>
#include <qtl_mysql.hpp>
#include <qtl_sqlite.hpp>

#include "OpenRS/database/models/player.h"
#include "OpenRS/manager/configmanager.h"
#include "common/log.h"
#include "options.h"

openrs::manager::DatabaseManager::~DatabaseManager() {
  if (this->db_mysql) this->db_mysql->close();
  if (this->db_sqlite) this->db_sqlite->close();
}

bool openrs::manager::DatabaseManager::Init() {
  const auto database_config =
      openrs::manager::ConfigManager::get()["database"];
  if (database_config["mode"].get<std::string>() == "mysql") {
    if (!this->InitMySQL()) return false;
    try {
      openrs::database::models::PlayerModel::CreateModelTable<qtl::mysql::database>(
          *this->db_mysql.get());
    } catch (const qtl::mysql::error& ex) {
      common::Log(common::Log::LogLevel::kError)
          << "[DatabaseManager] Failed to initialized database table: "
          << this->db_mysql->errmsg();
      return false;
    }
  } else {
    if (!this->InitSqlite()) return false;
    try {
      openrs::database::models::PlayerModel::CreateModelTable<qtl::sqlite::database>(
          *this->db_sqlite.get());
    } catch (const qtl::sqlite::error& ex) {
      common::Log(common::Log::LogLevel::kError)
          << "[DatabaseManager] Failed to initialized database table: "
          << this->db_sqlite->errmsg();
      return false;
    }
  }

  return true;
}

bool openrs::manager::DatabaseManager::InitMySQL() {
  const auto database_config =
      openrs::manager::ConfigManager::get()["database"];
  this->db_mysql = std::make_shared<qtl::mysql::database>();
  if (this->db_mysql->open(
          database_config["host"].get<std::string>().c_str(),
          database_config["username"].get<std::string>().c_str(),
          database_config["password"].get<std::string>().c_str(),
          database_config["database"].get<std::string>().c_str())) {
    common::Log(common::Log::LogLevel::kInfo)
        << "[DatabaseManager] Initialized mysql database "
        << database_config["host"].get<std::string>();
    return true;
  } else {
    common::Log(common::Log::LogLevel::kError)
        << "[DatabaseManager] Failed to initialized mysql database "
        << database_config["host"].get<std::string>();
    return false;
  }
}

bool openrs::manager::DatabaseManager::InitSqlite() {
  const auto database_config =
      openrs::manager::ConfigManager::get()["database"];
  this->db_sqlite = std::make_shared<qtl::sqlite::database>();
  try {
    this->db_sqlite->open(
        database_config["filename"].get<std::string>().c_str());
  } catch (const qtl::sqlite::error& ex) {
    common::Log(common::Log::LogLevel::kWarning)
        << "[DatabaseManager] Failed to init database: " << ex.what();
    return false;
  }

  common::Log(common::Log::LogLevel::kInfo)
      << "[DatabaseManager] Initialized sqlite database "
      << database_config["filename"].get<std::string>();
  return true;
}
