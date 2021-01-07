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

class SkillsColumnSet : public ColumnSet {
 public:
  int skill_attack;

 public:
  SkillsColumnSet() : skill_attack(0) {}
  SkillsColumnSet(const SkillsColumnSet& other)
      : ColumnSet(other), skill_attack(other.skill_attack) {}

 public:
  static inline const std::vector<
      std::tuple<std::string, ColumnSet::member_bind<SkillsColumnSet>>>
  GetColumnDefinitions() {
    return {
        {"skill_attack",
         std::bind(&SkillsColumnSet::skill_attack, std::placeholders::_1)},
    };
  }
};

}  // namespace columnsets
}  // namespace database
}  // namespace openrs
