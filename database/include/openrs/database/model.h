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
#include <variant>

#include "openrs/database/columnset.h"
#include "openrs/database/columnsets/id.h"

namespace openrs {
namespace database {

/**
 * Helper struct to swallow variadic template arguments.
 *
 * @tparam A generic list of templates.
 */
template <typename...>
struct TypeList {};

/**
 * Helper class used to describe a set of columns that build a model.
 *
 * @tparam Database The database object to work with.
 */
template <typename Database>
class ModelColumns {
 public:
  /**
   * Get a list of strings that describe a column set.
   *
   * @tparam ColumnSet The ColumnSet type to get descriptions from.
   * @tparam ColumnSets The remaining ColumnSet types to get descriptions from.
   * @return A vector of string describing the column name and type.
   */
  template <typename ColumnSet, typename... ColumnSets>
  inline static const std::vector<std::tuple<std::string, std::string>>
  GetColumnDefinitions(TypeList<ColumnSet, ColumnSets...>) {
    auto current = ColumnSet::template GetColumnDefinitions<Database>();
    auto remaining =
        ModelColumns::GetColumnDefinitions(TypeList<ColumnSets...>());
    current.insert(current.end(), remaining.cbegin(), remaining.cend());
    return current;
  }

  /**
   * Get a list of strings that describe a column set.
   *
   * @return A vector of string describing the column name and type.
   */
  inline static const std::vector<std::tuple<std::string, std::string>>
  GetColumnDefinitions(TypeList<>) {
    return {};
  }
};

/**
 * Helper class used to bind a set of columns that build a model to a database.
 *
 * @tparam Statement The database statement object to work with.
 */
template <typename Model, typename Statement>
class ModelBinds {
 public:
  /**
   * Binds a set of columns from a model to a statement.
   *
   * @tparam ColumnSet The ColumnSet type to get descriptions from.
   * @tparam ColumnSets The remaining ColumnSet types to get descriptions from.
   * @return A vector of string describing the column name and type.
   */
  template <typename ColumnSet, typename... ColumnSets>
  inline static void BindColumns(Model& model, Statement& command,
                                 uint32_t bind_index,
                                 TypeList<ColumnSet, ColumnSets...>) {
    const auto& kColumns = ColumnSet::GetColumnBinds();
    for (auto& kColumn : kColumns) {
      std::visit(
          [&model, &kColumn, &command, &bind_index](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T,
                                         openrs::database::ColumnSet::
                                             member_bind_string<ColumnSet>>) {
              qtl::bind_param<Statement>(command, bind_index++, arg(model));
            } else if constexpr (std::is_same_v<
                                     T, openrs::database::ColumnSet::
                                            member_bind_int<ColumnSet>>) {
              qtl::bind_param<Statement>(command, bind_index++, arg(model));
            } else {
              static_assert(always_false_v<T>,
                            "unknown model column bind type");
            }
          },
          std::get<1>(kColumn));
    }

    ModelBinds<Model, Statement>::BindColumns(model, command, bind_index,
                                              TypeList<ColumnSets...>());
  }

  /**
   * Get a list of strings that describe a column set.
   *
   * @return A vector of string describing the column name and type.
   */
  inline static void BindColumns(Model& model, Statement& command,
                                 uint32_t bind_index, TypeList<>) {
    return;
  }
};

class Model : public openrs::database::columnsets::IDColumnSet {
 public:
  Model() : openrs::database::columnsets::IDColumnSet() {}

 protected:
  /**
   * Get a list of strings that describe a columnset.
   *
   * @tparam Database The database object to work with.
   * @tparam ColumnSets The ColumnSet types to get descriptions from.
   * @return A vector of string describing the column name and type.
   */
  template <typename Database, typename... ColumnSets>
  inline static const std::vector<std::tuple<std::string, std::string>>
  GetAllColumnDefinitions() {
    return ModelColumns<Database>::template GetColumnDefinitions<ColumnSets...>(
        TypeList<ColumnSets...>());
  }

 protected:
  /**
   * Creates a table in the database if it doesn't already exist.
   *
   * @tparam Database The type of database to use.
   * @tparam ColumnSets The ColumnSet types to get column information from.
   *  This should not include the IDColumnSet as it is added automatically.
   * @param database The database object itself.
   * @param kTableName The name of the table to create.
   * @return True on success, false otherwise.
   */
  template <typename Database, typename... ColumnSets>
  inline static bool CreateTable(Database& database,
                                 const std::string& kTableName) {
    const auto kColumns = Model::template GetAllColumnDefinitions<
        Database, openrs::database::columnsets::IDColumnSet, ColumnSets...>();

    std::string columns;
    for (auto column = kColumns.cbegin(); column != kColumns.cend(); ++column) {
      columns += std::get<0>(*column) + " " + std::get<1>(*column);
      if (column != kColumns.cend() - 1) {
        columns += ",";
      }
    }

    auto statement = database.open_command("CREATE TABLE IF NOT EXISTS " +
                                           kTableName + " (" + columns + ");");
    return statement.fetch();
  }

  /**
   * Inserts a row into a table in the database.
   *
   * @tparam Model The type of model to use when pulling column definitions.
   * @tparam Database The type of database to use.
   * @tparam Statement The database statement type to bind parameters to.
   * @tparam ColumnSets The ColumnSet types to get column information from.
   *  This should not include the IDColumnSet as it is added automatically.
   * @param model The actual model object to insert.
   * @param database The database object itself.
   * @param kTableName The name of the table to insert the row into.
   * @return True on success, false otherwise.
   */
  template <typename Model, typename Database, typename Statement,
            typename... ColumnSets>
  inline static bool Insert(Model& model, Database& database,
                            const std::string& kTableName) {
    const auto kColumns =
        Model::template GetAllColumnDefinitions<Database, ColumnSets...>();

    std::string column_names;
    std::string column_wildcards;
    for (auto column = kColumns.cbegin(); column != kColumns.cend(); ++column) {
      column_names += std::get<0>(*column);
      column_wildcards += "?";
      if (column != kColumns.cend() - 1) {
        column_names += ",";
        column_wildcards += ",";
      }
    }

    auto statement = database.open_command("INSERT INTO " + kTableName + " (" +
                                           column_names + ") VALUES (" +
                                           column_wildcards + ");");
    ModelBinds<Model, Statement>::template BindColumns<ColumnSets...>(
        model, statement, 0, TypeList<ColumnSets...>());

    // INSERT returns a false result.
    if (statement.fetch()) {
      return false;
    }

    model.id = statement.insert_id();
    return true;
  }

  /**
   * Updates a row inside a table in the database.
   *
   * @tparam Model The type of model to use when pulling column definitions.
   * @tparam Database The type of database to use.
   * @tparam Statement The database statement type to bind parameters to.
   * @tparam ColumnSets The ColumnSet types to get column information from.
   *  This should not include the IDColumnSet as it is added automatically.
   * @param model The actual model object to insert.
   * @param database The database object itself.
   * @param kTableName The name of the table to insert the row into.
   * @return True on success, false otherwise.
   */
  template <typename Model, typename Database, typename Statement,
            typename... ColumnSets>
  inline static bool Update(Model& model, Database& database,
                            const std::string& kTableName) {
    const auto kColumns =
        Model::template GetAllColumnDefinitions<Database, ColumnSets...>();

    std::string column_names;
    for (auto column = kColumns.cbegin(); column != kColumns.cend(); ++column) {
      column_names += std::get<0>(*column) + "=?";
      if (column != kColumns.cend() - 1) {
        column_names += ",";
      }
    }

    auto statement = database.open_command("UPDATE " + kTableName + " SET " +
                                           column_names + " WHERE id=?;");
    ModelBinds<Model, Statement>::template BindColumns<ColumnSets...>(
        model, statement, 0, TypeList<ColumnSets...>());
    ModelBinds<Model, Statement>::template BindColumns<
        openrs::database::columnsets::IDColumnSet>(
        model, statement, kColumns.size(),
        TypeList<openrs::database::columnsets::IDColumnSet>());

    // INSERT returns a false result.
    if (statement.fetch()) {
      return false;
    }

    return true;
  }
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
