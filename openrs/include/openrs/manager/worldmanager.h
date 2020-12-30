#pragma once

#include <inttypes.h>
#include <openrs/common/io/buffer.h>
#include <openrs/common/singleton.h>
#include <openrs/game/world.h>

#include <unordered_map>

#include "openrs/manager/manager.h"
#include "openrs/net/session.h"

namespace openrs {
namespace manager {

class WorldManager : public openrs::manager::Manager,
                     public openrs::common::Singleton<WorldManager> {
 private:
  std::unordered_map<uint32_t, openrs::game::World> worlds_;

 public:
  WorldManager(void) {}

  bool Init() override;

  void GetLocalPlayerUpdate(
      const uint32_t& kWorldId,
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      openrs::net::Session* session,
      openrs::common::io::Buffer<>* buffer) const;

  void SendMapRegion(const std::shared_ptr<openrs::game::Player>& kPlayer,
                     openrs::net::Session* session,
                     const bool kSendLocalPlayerUpdate = false) const;

  inline const auto& worlds() const { return this->worlds_; }
  inline void add_world(const uint32_t& id, const openrs::game::World& world) {
    this->worlds_.insert_or_assign(id, world);
  }
  inline void add_world(const uint32_t& id, openrs::game::World&& world) {
    this->worlds_.insert_or_assign(id, std::move(world));
  }

  inline uint32_t add_player(
      const uint32_t& kWorldId,
      const std::shared_ptr<openrs::game::Player>& kPlayer) {
    return this->worlds_[kWorldId].add_player(kPlayer);
  }
  inline void remove_player(const uint32_t& kWorldId,
                            const uint32_t& kPlayerId) {
    this->worlds_[kWorldId].remove_player(kPlayerId);
  }
  inline void remove_player(
      const uint32_t& kWorldId,
      const std::shared_ptr<openrs::game::Player>& kPlayer) {
    this->remove_player(kWorldId, kPlayer->id);
  }
};

}  // namespace manager
}  // namespace openrs
