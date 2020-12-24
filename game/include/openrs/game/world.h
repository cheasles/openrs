#pragma once

#include <inttypes.h>

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
  std::unordered_map<uint32_t, std::shared_ptr<openrs::game::Player>> players_;
  std::unordered_map<uint32_t, openrs::game::Entity> npcs_;

 public:
  World() : world_type_(WorldType::kFree) {}

  inline const auto& world_type() const { return this->world_type_; }
  inline const auto& npcs() const { return this->npcs_; }
  inline const auto& players() const { return this->players_; }

  inline void add_player(const std::shared_ptr<openrs::game::Player>& kPlayer) {
    this->players_.insert_or_assign(static_cast<uint32_t>(kPlayer->id),
                                    kPlayer);
  }
  inline void remove_player(const uint32_t& kPlayerId) {
    this->players_.erase(kPlayerId);
  }
  inline void remove_player(
      const std::shared_ptr<openrs::game::Player>& kPlayer) {
    this->players_.erase(kPlayer->id);
  }
};

}  // namespace game
}  // namespace openrs
