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

class PlayerColumnSet : public ColumnSet {
 public:
  std::string username;
  std::string password;
  std::string salt;
  int rights;

 public:
  PlayerColumnSet() : rights(0) {}
  PlayerColumnSet(const PlayerColumnSet& other)
      : ColumnSet(other),
        username(other.username),
        password(other.password),
        salt(other.salt),
        rights(other.rights) {}

 public:
  static inline const std::vector<
      std::tuple<std::string, ColumnSet::member_bind<PlayerColumnSet>>>
  GetColumnDefinitions() {
    return {
        {"username",
         std::bind(&PlayerColumnSet::username, std::placeholders::_1)},
        {"password",
         std::bind(&PlayerColumnSet::password, std::placeholders::_1)},
        {"salt", std::bind(&PlayerColumnSet::salt, std::placeholders::_1)},
        {"rights", std::bind(&PlayerColumnSet::rights, std::placeholders::_1)},
    };
  }
};

}  // namespace columnsets
}  // namespace database
}  // namespace openrs
