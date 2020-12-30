#pragma once

#include <inttypes.h>

#include <map>
#include <unordered_map>

#include "openrs/game/entity.h"
#include "openrs/game/player.h"

namespace openrs {
namespace game {

class World {
 public:
  enum struct WorldType : uint8_t {
    kFree = 0,
    kMembers = 1,
    kSubscription = 2
  };

 private:
  WorldType world_type_;
  std::map<uint32_t, std::shared_ptr<openrs::game::Player>> players_;
  std::unordered_map<uint32_t, openrs::game::Entity> npcs_;

 public:
  World() : world_type_(WorldType::kMembers) {}

  inline const auto& world_type() const { return this->world_type_; }
  inline const auto& npcs() const { return this->npcs_; }
  inline const auto& players() const { return this->players_; }

  inline uint32_t add_player(
      const std::shared_ptr<openrs::game::Player>& kPlayer) {
    uint32_t index = 0;
    // Check if there are any gaps in the list:
    if (this->players_.empty() ||
        this->players_.crbegin()->first == this->players_.size() - 1) {
      // No gaps, insert new player at the end.
      index =
          this->players_
              .insert(
                  this->players_.end(),
                  std::pair<uint32_t, std::shared_ptr<openrs::game::Player>>(
                      static_cast<uint32_t>(this->players_.size()), kPlayer))
              ->first;
    } else {
      // There are gaps. Find an empty slot to insert the new player.
      for (const auto& kIndex : this->players_) {
        if (kIndex.first != index++) {
          break;
        }
      }
      index =
          this->players_
              .insert(
                  this->players_.end(),
                  std::pair<uint32_t, std::shared_ptr<openrs::game::Player>>(
                      static_cast<uint32_t>(index), kPlayer))
              ->first;
    }

    return index;
  }
  inline void remove_player(const uint32_t& kPlayerIndex) {
    this->players_.erase(kPlayerIndex);
  }
};

}  // namespace game
}  // namespace openrs
