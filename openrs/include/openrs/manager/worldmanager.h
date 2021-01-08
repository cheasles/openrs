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

#include <inttypes.h>
#include <openrs/common/event/eventsink.h>
#include <openrs/common/io/bitbuffer.h>
#include <openrs/common/singleton.h>
#include <openrs/game/player/skills.h>
#include <openrs/game/world.h>

#include <unordered_map>

#include "openrs/event/login.h"
#include "openrs/manager/manager.h"
#include "openrs/net/session.h"

namespace openrs {
namespace manager {

/**
 * Manages each world the server is currently hosting.
 */
class WorldManager
    : public openrs::manager::Manager,
      public openrs::common::Singleton<WorldManager>,
      public openrs::common::event::EventSink<openrs::event::EventLogin> {
 public:
  /**
   * Supported message types to send to clients.
   */
  enum struct MessageType : uint16_t {
    kDefault = 0,
  };

  /**
   * Music track IDs.
   */
  enum struct MusicTrackID : uint16_t {

  };

  /**
   * Music effect IDs.
   */
  enum struct MusicEffectID : uint16_t {

  };

 private:
  std::unordered_map<uint32_t, openrs::game::World> worlds_;

 public:
  WorldManager(void) {}

  /**
   * Creates the required world objects for us to manage.
   *
   * @return Always returns true.
   */
  bool Init() override;

  /**
   * Updates each object that is managed by this instance.
   *
   * @param kTimeSinceLastUpdate The amount of time that has passed since the
   *  last update.
   * @return True on a successful update, false otherwise.
   */
  bool Update(const std::chrono::milliseconds& kTimeSinceLastUpdate) override;

  /**
   * Handles a player login event.
   *
   * @param kEvent The event details.
   * @return True if event processing should continue to other event handlers,
   *  or false to stop handling this event.
   */
  bool HandleEvent(const openrs::event::EventLogin& kEvent) override;

  /**
   * Starts a game session for a player.
   *
   * When a player first logs into the server, we have to send a few different
   * bits of information to them. This method calls all the appropriate parts
   * to make sure the client gets everything they need to start.
   *
   * @note The player object should have already been added to the world using
   *  `WorldManager::add_player`.
   *
   * @param kPlayer The player to generate map data for.
   * @param session The client session to send the data to.
   */
  void StartPlayer(const std::shared_ptr<openrs::game::Player>& kPlayer,
                   std::shared_ptr<openrs::net::Session> session) const;

  /**
   * Populate a buffer with information about local players.
   *
   * @note Players are considered local to one another if they are within a
   *  short distance. This distance changes depending on the player's viewport.
   *
   * @param kWorldId The world the player belongs to.
   * @param kPlayerIndex The current player index.
   * @param kPlayer The player to find other local players for.
   * @param buffer The buffer object to populate.
   */
  void GetLocalPlayerUpdate(
      const uint32_t& kWorldId, const uint32_t& kPlayerIndex,
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      openrs::common::io::BitBuffer<>* buffer) const;

  /**
   * Sends either a dynamic or fixed map to the client.
   *
   * @param kPlayer The player to generate map data for.
   * @param session The client session to send the data to.
   * @param kSendLocalPlayerUpdate Send the local player information too.
   */
  inline void SendMap(const std::shared_ptr<openrs::game::Player>& kPlayer,
                      std::shared_ptr<openrs::net::Session>& session,
                      const bool kSendLocalPlayerUpdate = false) const {
    if (false) {
      this->SendDynamicMapRegion(kPlayer, session, kSendLocalPlayerUpdate);
    } else {
      this->SendMapRegion(kPlayer, session, kSendLocalPlayerUpdate);
    }

    kPlayer->set_force_next_map_load_refresh(false);
  }

  /**
   * Sends fixed map to the client.
   *
   * @param kPlayer The player to generate map data for.
   * @param session The client session to send the data to.
   * @param kSendLocalPlayerUpdate Send the local player information too.
   */
  void SendMapRegion(const std::shared_ptr<openrs::game::Player>& kPlayer,
                     std::shared_ptr<openrs::net::Session>& session,
                     const bool kSendLocalPlayerUpdate = false) const;

  /**
   * Sends a dynamic map to the client.
   *
   * @param kPlayer The player to generate map data for.
   * @param session The client session to send the data to.
   * @param kSendLocalPlayerUpdate Send the local player information too.
   */
  void SendDynamicMapRegion(
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      std::shared_ptr<openrs::net::Session>& session,
      const bool kSendLocalPlayerUpdate = false) const;

  /**
   * Sends information about local players to the client.
   *
   * @param kPlayer The player to generate map data for.
   * @param session The client session to send the data to.
   */
  void SendLocalPlayerUpdate(
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      std::shared_ptr<openrs::net::Session>& session) const;

  /**
   * Sends the players run energy to them.
   *
   * @param kPlayer The player to get run energy data for.
   * @param session The client session to send the data to.
   */
  void SendRunEnergy(const std::shared_ptr<openrs::game::Player>& kPlayer,
                     std::shared_ptr<openrs::net::Session>& session) const;

  /**
   * Sends the players hitpoints to them.
   *
   * @param kPlayer The player to get hitpoint data for.
   * @param session The client session to send the data to.
   */
  void SendPlayerHitPoints(
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      std::shared_ptr<openrs::net::Session>& session) const;

  /**
   * Sends the item look packet to the client.
   *
   * @param kPlayer The player to get the item look information from.
   * @param session The client session to send the data to.
   */
  void SendItemLook(const std::shared_ptr<openrs::game::Player>& kPlayer,
                    std::shared_ptr<openrs::net::Session>& session) const;

  /**
   * Sends custom packet to the client.
   *
   * @param kPlayer The player to generate map data for.
   * @param session The client session to send the data to.
   */
  void SendCustom161(const std::shared_ptr<openrs::game::Player>& kPlayer,
                     std::shared_ptr<openrs::net::Session>& session) const;

  /**
   * Sends a text message to the client.
   *
   * @param kPlayer The player to send the message to.
   * @param session The client session to send the data to.
   * @param kMessageType The type of message to send.
   * @param kMessage The string message to send.
   */
  void SendMessage(const std::shared_ptr<openrs::game::Player>& kPlayer,
                   std::shared_ptr<openrs::net::Session>& session,
                   const MessageType& kMessageType,
                   const std::string& kMessage) const;

  /**
   * Sends a text message to the client.
   *
   * @param kPlayer The player to grab map information from.
   * @param session The client session to send the data to.
   * @param kTile The tile to send.
   */
  void SendCreateWorldTile(const std::shared_ptr<openrs::game::Player>& kPlayer,
                           std::shared_ptr<openrs::net::Session>& session,
                           const openrs::game::WorldTile& kTile) const;

  /**
   * Sends a music track to the client.
   *
   * @param kPlayer The player to send interface settings to.
   * @param session The client session to send the data to.
   * @param kMusicId The track ID.
   * @param kDelay How long to wait before the client plays the track.
   * @param kVolume The volumne to play the track at. 255 is max volume.
   */
  void SendMusic(const std::shared_ptr<openrs::game::Player>& player,
                 std::shared_ptr<openrs::net::Session>& session,
                 const MusicTrackID& kMusicId, const uint8_t& kDelay = 100,
                 const uint8_t& kVolume = 255) const;

  /**
   * Sends a music effect to the client.
   *
   * @param kPlayer The player to send interface settings to.
   * @param session The client session to send the data to.
   * @param kMusicId The effect ID.
   * @param kVolume The volumne to play the effect at. 255 is max volume.
   */
  void SendMusicEffect(const std::shared_ptr<openrs::game::Player>& player,
                       std::shared_ptr<openrs::net::Session>& session,
                       const MusicEffectID& kMusicId,
                       const uint8_t& kVolume = 255) const;

  /**
   * Inform the client whether or not they're in a multi-combat area.
   *
   * @param kPlayer The player to check the combat area type for.
   * @param session The client session to send the data to.
   */
  void SendMultiCombatArea(
      const std::shared_ptr<openrs::game::Player>& kPlayer,
      std::shared_ptr<openrs::net::Session>& session) const;

  /**
   * Sends player skill information to a client.
   *
   * @param kPlayer The player to send interface settings to.
   * @param session The client session to send the data to.
   * @param kSkill The skill to send to the player.
   */
  void SendPlayerSkill(const std::shared_ptr<openrs::game::Player>& kPlayer,
                       std::shared_ptr<openrs::net::Session>& session,
                       const openrs::game::player::Skills::Skill& kSkill) const;

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
