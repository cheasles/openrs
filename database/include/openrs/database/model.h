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
