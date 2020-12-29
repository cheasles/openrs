#pragma once

#include <qtl_common.hpp>
#include <qtl_mysql.hpp>
#include <qtl_sqlite.hpp>

#include "openrs/database/columnset.h"
#include "openrs/database/columnsets/id.h"

namespace openrs {
namespace database {

class Model : public openrs::database::columnsets::IDColumnSet {
 public:
  Model() : openrs::database::columnsets::IDColumnSet() {}
};

}  // namespace database
}  // namespace openrs

namespace qtl {

template <>
inline void bind_record<qtl::mysql::statement, openrs::database::Model>(
    qtl::mysql::statement& command, openrs::database::Model&& v) {
  openrs::database::ColumnSet::BindFields<
      qtl::mysql::statement, openrs::database::columnsets::IDColumnSet>(command,
                                                                        v);
}

template <>
inline void bind_record<qtl::sqlite::statement, openrs::database::Model>(
    qtl::sqlite::statement& command, openrs::database::Model&& v) {
  openrs::database::ColumnSet::BindFields<
      qtl::sqlite::statement, openrs::database::columnsets::IDColumnSet>(
      command, v);
}

}  // namespace qtl
