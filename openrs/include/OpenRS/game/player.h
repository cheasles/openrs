#pragma once

#include "OpenRS/database/models/player.h"

namespace openrs {
namespace game {

class Player : public database::models::PlayerModel {
 public:
  bool CheckPassword(const std::vector<uint8_t>& kPassword,
                     const uint32_t& kIterations = 10) const;
};

}  // namespace game
}  // namespace openrs
