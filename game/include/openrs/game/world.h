#pragma once

#include <inttypes.h>

#include <unordered_map>

#include "openrs/game/entity.h"
#include "openrs/game/player.h"

namespace openrs {
namespace game {

class World {
 private:
  std::unordered_map<uint32_t, openrs::game::Player> players_;
  std::unordered_map<uint32_t, openrs::game::Entity> npcs_;

 public:
  inline const auto& npcs() const { return this->npcs_; }
  inline const auto& players() const { return this->players_; }

  inline void add_player(const openrs::game::Player& kPlayer) {
    this->players_.insert_or_assign(static_cast<uint32_t>(kPlayer.id), kPlayer);
  }
  inline void add_player(openrs::game::Player&& player) {
    this->players_.insert_or_assign(static_cast<uint32_t>(player.id),
                                    std::move(player));
  }
  inline void remove_player(const uint32_t& kPlayerId) {
      this->players_.erase(kPlayerId);
  }
  inline void remove_player(const openrs::game::Player& kPlayer) {
      this->players_.erase(kPlayer.id);
  }
};

}  // namespace game
}  // namespace openrs
