#pragma once

#include <common/io/buffer.h>
#include <inttypes.h>

#include <memory>
#include <nlohmann/json.hpp>
#include <qtl_mysql.hpp>
#include <qtl_sqlite.hpp>

#include "OpenRS/manager/manager.h"
#include "common/singleton.h"

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

 private:
  bool InitMySQL();
  bool InitSqlite();
};

}  // namespace manager

}  // namespace openrs
