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

#include <functional>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "openrs/database/columnset.h"

namespace openrs {
namespace database {
namespace columnsets {

/**
 * A set of columns related to general player information.
 */
class PlayerColumnSet : public ColumnSet {
 public:
  /**
   * The player's username.
   */
  std::string username;

  /**
   * The player's hex-encoded hashed password.
   */
  std::string password;

  /**
   * The player's unique salt used when hashing thier password.
   */
  std::string salt;

  /**
   * The player's rights on the server.
   *
   * Moderators should have this value set to 1, while administrators to 2.
   */
  int rights;

  /**
   * How much time is remaining before the player's skull expires.
   */
  int skull_time_left;

 public:
  PlayerColumnSet() : rights(0), skull_time_left(0) {}
  PlayerColumnSet(const PlayerColumnSet& other)
      : ColumnSet(other),
        username(other.username),
        password(other.password),
        salt(other.salt),
        rights(other.rights),
        skull_time_left(other.skull_time_left) {}

 public:
  /**
   * Get a list of the column names and their associated member variables.
   *
   * @return a vector of tuples containing the column name and the member
   *  variable for binding.
   */
  static inline const std::vector<
      std::tuple<std::string, ColumnSet::member_bind<PlayerColumnSet>>>
  GetColumnBinds() {
    return {
        {"username",
         std::bind(&PlayerColumnSet::username, std::placeholders::_1)},
        {"password",
         std::bind(&PlayerColumnSet::password, std::placeholders::_1)},
        {"salt", std::bind(&PlayerColumnSet::salt, std::placeholders::_1)},
        {"rights", std::bind(&PlayerColumnSet::rights, std::placeholders::_1)},
        {"skull_time_left",
         std::bind(&PlayerColumnSet::skull_time_left, std::placeholders::_1)},
    };
  }

  /**
   * Get a list of the column names and their associated database properties.
   *
   * @tparam Database The database object to work with. Used for
   *  specialization.
   * @return a vector of tuples containing the column name and the column
   *  description depending on the type of database specified.
   */
  template <typename Database>
  static const std::vector<std::tuple<std::string, std::string>>
  GetColumnDefinitions();
};

}  // namespace columnsets
}  // namespace database
}  // namespace openrs
