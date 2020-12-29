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

class IDColumnSet : public ColumnSet {
 public:
  int id;

 public:
  IDColumnSet() : id(0) {}

 public:
  static inline const std::vector<
      std::tuple<std::string, ColumnSet::member_bind<IDColumnSet>>>
  GetColumnDefinitions() {
    return {{"id", std::bind(&IDColumnSet::id, std::placeholders::_1)}};
  }
};

}  // namespace columnsets
}  // namespace database
}  // namespace openrs
