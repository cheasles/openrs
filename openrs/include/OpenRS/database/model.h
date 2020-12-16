#pragma once

#include <endian.h>

#include <fstream>
#include <qtl_common.hpp>
#include <qtl_mysql.hpp>
#include <qtl_sqlite.hpp>
#include <string>
#include <vector>

namespace openrs {
namespace database {

struct Model {
  int id;
};

}  // namespace database
}  // namespace openrs

namespace qtl {

template <>
inline void bind_record<qtl::mysql::statement, openrs::database::Model>(
    qtl::mysql::statement& command, openrs::database::Model&& v) {
  qtl::bind_field(command, "id", v.id);
}

template <>
inline void
bind_record<qtl::sqlite::statement, openrs::database::Model>(
    qtl::sqlite::statement& command, openrs::database::Model&& v) {
  qtl::bind_field(command, "id", v.id);
}

}  // namespace qtl
