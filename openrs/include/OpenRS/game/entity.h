#pragma once

#include <inttypes.h>

#include "OpenRS/game/worldtile.h"

namespace openrs {
namespace game {

class Entity : public openrs::game::WorldTile {
 public:
  enum struct Direction : uint8_t { kNorth, kEast, kSouth, kWest };

 private:
  Direction direction_;

 public:
  Entity() : direction_(Direction::kNorth) {}

  inline auto direction() const { return this->direction_; }
};

}  // namespace game
}  // namespace openrs
