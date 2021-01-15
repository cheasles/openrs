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
#include "openrs/manager/worldmanager.h"

#include <openrs/common/io/buffer.h>
#include <openrs/common/log.h>

#include <bitset>

#include "openrs/manager/appearancemanager.h"
#include "openrs/manager/configmanager.h"
#include "openrs/manager/interfacemanager.h"

bool openrs::manager::WorldManager::Init() {
  this->add_world(1, openrs::game::World());
  this->add_sink(openrs::manager::WorldManager::get());

  common::Log(common::Log::LogLevel::kInfo)
      << "[WorldManager] Initialized " << this->worlds_.size() << " worlds.";
  return true;
}

bool openrs::manager::WorldManager::HandleEvent(
    const openrs::event::EventLogin& kEvent) {
  this->sessions_.insert(
      std::make_pair<uint32_t, std::weak_ptr<openrs::net::Session>>(
          std::get<1>(kEvent)->player_index(), std::get<1>(kEvent)));
  this->StartPlayer(std::get<0>(kEvent), std::get<1>(kEvent));
  this->EmitEvent({std::get<0>(kEvent), openrs::game::WorldTile(0, 0, 0)});
  return true;
}

bool openrs::manager::WorldManager::HandleEvent(
    const openrs::event::entity::EventEntityMove& kEvent) {
  const auto& kEntity = std::get<0>(kEvent);
  const auto& kPreviousPosition = std::get<1>(kEvent);

  const auto& kWorld = this->worlds_.find(1);
  if (this->worlds_.cend() == kWorld) {
    return false;
  }

  for (const auto& kPlayer : kWorld->second.players()) {
    const auto& kSession = this->sessions_.find(kPlayer.first);
    if (this->sessions_.cend() == kSession) {
      // TODO: Cleanup player object.
      continue;
    }

    if (auto session = kSession->second.lock()) {
      openrs::common::io::BitBuffer<> buffer;
      openrs::common::io::Buffer<> appearance_buffer;

      if (kPlayer.second->WithinDistance(kPreviousPosition) &&
          !kPlayer.second->WithinDistance(*kEntity)) {
        // The entity has moved out of view of this player.
        buffer.PutData<uint8_t>(1, 1);
        buffer.PutData<uint8_t>(1, 0);
        buffer.PutData<uint8_t>(2, 0);

        // TODO: Region matching
        buffer.PutData<uint8_t>(1, 0);
      } else if (!kPlayer.second->WithinDistance(kPreviousPosition) &&
                 kPlayer.second->WithinDistance(*kEntity)) {
        // The entity has moved into view of this player.
        buffer.PutData<uint8_t>(1, 1);
        buffer.PutData<uint8_t>(1, 1);
        buffer.PutData<uint8_t>(2, 0);

        uint32_t data_mask = 0x40000 | 0x10;
        if (kPlayer.second->face_entity_last() != -2) {
          data_mask |= 0x1;
        }
        if (kPlayer.second->graphics()[0] != 0) {
          data_mask |= 0x4;
        }
        if (kPlayer.second->graphics()[1] != 0) {
          data_mask |= 0x8000;
        }
        if (kPlayer.second->graphics()[2] != 0) {
          data_mask |= 0x400000;
        }
        if (kPlayer.second->graphics()[3] != 0) {
          data_mask |= 0x800000;
        }
        if (kPlayer.second->update_movement_type()) {
          data_mask |= 0x80;
        }

        if (data_mask >= std::numeric_limits<uint8_t>::max()) {
          data_mask |= 0x2;
        }
        if (data_mask >= std::numeric_limits<uint16_t>::max()) {
          data_mask |= 0x400;
        }
        appearance_buffer.PutData(static_cast<uint8_t>(data_mask));
        if (data_mask > std::numeric_limits<uint8_t>::max()) {
          appearance_buffer.PutData(static_cast<uint8_t>(data_mask >> 8));
        }
        if (data_mask > std::numeric_limits<uint16_t>::max()) {
          appearance_buffer.PutData(static_cast<uint8_t>(data_mask >> 16));
        }

        if (kPlayer.second->graphics()[0] != 0) {
          appearance_buffer.PutDataBE<uint16_t>(
              kPlayer.second->graphics()[0].id());
          appearance_buffer.PutDataVBE(
              kPlayer.second->graphics()[0].GetSettingsHash());
          appearance_buffer.PutShiftedPosDataBE(
              kPlayer.second->graphics()[0].GetSettingsHash2());
        }
        if (kPlayer.second->graphics()[1] != 0) {
          appearance_buffer.PutDataBE<uint16_t>(
              kPlayer.second->graphics()[1].id());
          appearance_buffer.PutDataVBE(
              kPlayer.second->graphics()[1].GetSettingsHash());
          appearance_buffer.PutShiftedPosDataBE(
              kPlayer.second->graphics()[1].GetSettingsHash2());
        }
        if (kPlayer.second->graphics()[2] != 0) {
          appearance_buffer.PutDataBE<uint16_t>(
              kPlayer.second->graphics()[2].id());
          appearance_buffer.PutDataVBE(
              kPlayer.second->graphics()[2].GetSettingsHash());
          appearance_buffer.PutShiftedPosDataBE(
              kPlayer.second->graphics()[2].GetSettingsHash2());
        }
        if (kPlayer.second->graphics()[3] != 0) {
          appearance_buffer.PutDataBE<uint16_t>(
              kPlayer.second->graphics()[3].id());
          appearance_buffer.PutDataVBE(
              kPlayer.second->graphics()[3].GetSettingsHash());
          appearance_buffer.PutShiftedPosDataBE(
              kPlayer.second->graphics()[3].GetSettingsHash2());
        }
        openrs::manager::AppearanceManager::GetPlayerAppearance(
            kPlayer.second, &appearance_buffer);
        if (kPlayer.second->face_entity_last() != -2) {
          appearance_buffer.PutShiftedPosDataBE(
              kPlayer.second->face_entity_last());
        }

        // Clan.
        appearance_buffer.PutData<uint8_t>(0);

        if (kPlayer.second->update_movement_type()) {
          appearance_buffer.PutShiftedPosDataBE(
              kPlayer.second->is_running() ? 2 : 1);
        }
      }

      buffer.insert(buffer.end(), appearance_buffer.begin(),
                    appearance_buffer.end());

      openrs::net::codec::Packet packet;
      packet.type = openrs::net::codec::PacketType::kLocalPlayerUpdate;
      packet.data = buffer;
      session->Send(packet);
    }
  }

  // Return false to allow other event sinks to handle this type too.
  return false;
}

bool openrs::manager::WorldManager::HandleEvent(
    const openrs::event::packet::EventPacketScreen& kEvent) {
  const auto& kWorld = this->worlds_.find(std::get<0>(kEvent)->player_world());
  if (this->worlds_.cend() == kWorld) {
    return false;
  }

  const auto& kPlayer =
      kWorld->second.players().find(std::get<0>(kEvent)->player_index());
  if (kWorld->second.players().cend() == kPlayer) {
    return false;
  }

  kPlayer->second->set_display_mode(std::get<1>(kEvent));
  kPlayer->second->set_screen_width(std::get<2>(kEvent));
  kPlayer->second->set_screen_height(std::get<3>(kEvent));

  const auto& interface_manager = openrs::manager::InterfaceManager::get();
  interface_manager->SendInterfaces(kPlayer->second, std::get<0>(kEvent));
  return true;
}

void openrs::manager::WorldManager::StartPlayer(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    std::shared_ptr<openrs::net::Session> session) const {
  const auto& kInterfaceManager = openrs::manager::InterfaceManager::get();
  const auto& kConfigManager = openrs::manager::ConfigManager::get();

  this->SendMap(kPlayer, session, true);
  kInterfaceManager->SendInterfaces(kPlayer, session);
  this->SendRunEnergy(kPlayer, session);
  this->SendPlayerHitPoints(kPlayer, session);
  this->SendItemLook(kPlayer, session);
  this->SendCustom161(kPlayer, session);
  this->SendMessage(
      kPlayer, session, openrs::manager::WorldManager::MessageType::kDefault,
      "Welcome to " + (*kConfigManager)["server"]["name"].get<std::string>());
  this->SendMessage(
      kPlayer, session, openrs::manager::WorldManager::MessageType::kDefault,
      (*kConfigManager)["server"]["name"].get<std::string>() +
          " is distributed under the GNU AGPL license. Please visit " +
          (*kConfigManager)["server"]["source_url"].get<std::string>() +
          " to view the source code.");

  this->SendMultiCombatArea(kPlayer, session);
  kConfigManager->SendConfig(
      kPlayer, session, openrs::manager::ConfigManager::Config::k281, 1000);
  kConfigManager->SendConfig(kPlayer, session,
                             openrs::manager::ConfigManager::Config::k1159, 1);
  kConfigManager->SendConfig(kPlayer, session,
                             openrs::manager::ConfigManager::Config::k1160, -1);
  kConfigManager->SendGameBarStages(kPlayer, session);

  for (std::underlying_type_t<openrs::game::player::Skills::Skill> skill = 0;
       skill !=
       static_cast<std::underlying_type_t<openrs::game::player::Skills::Skill>>(
           openrs::game::player::Skills::Skill::kLast);
       ++skill) {
    this->SendPlayerSkill(
        kPlayer, session,
        static_cast<openrs::game::player::Skills::Skill>(skill));
  }
}

void openrs::manager::WorldManager::GetLocalPlayerUpdate(
    const uint32_t& kWorldId, const uint32_t& kPlayerIndex,
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    openrs::common::io::BitBuffer<>* buffer) const {
  buffer->resize((30 + 2046 * 18 + 7) / 8);
  buffer->PutData(30, kPlayer->tile_hash());
  for (const auto& kLocalPlayer : this->worlds_.at(kWorldId).players()) {
    if (kLocalPlayer.first == kPlayerIndex) {
      continue;
    }

    buffer->seek(SEEK_SET, 30 + 18 * kLocalPlayer.first);
    buffer->PutData(18, kLocalPlayer.second->region_hash());
  }
}

void openrs::manager::WorldManager::SendMapRegion(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    const std::shared_ptr<openrs::net::Session>& session,
    const bool kSendLocalPlayerUpdate) const {
  openrs::common::io::BitBuffer<> buffer;
  if (kSendLocalPlayerUpdate) {
    this->GetLocalPlayerUpdate(1, session->player_index(), kPlayer, &buffer);
  }
  buffer.PutData<uint8_t>(-1 * kPlayer->map_size());
  buffer.PutShiftedNegDataBE<uint8_t>(
      kPlayer->force_next_map_load_refresh() ? 1 : 0);
  buffer.PutDataBE<uint16_t>(kPlayer->chunk_x());
  buffer.PutDataBE<uint16_t>(kPlayer->chunk_y());

  std::vector<uint32_t> map_regions;
  const auto& kCacheConfig =
      openrs::manager::ConfigManager::get()->cache_config().at(718);
  kPlayer->GetMapRegions(
      openrs::game::WorldTile::MAP_SIZES[kPlayer->map_size()] >> 4,
      &map_regions);
  for (const auto& kMapRegion : map_regions) {
    if (kCacheConfig["map_archive_keys"].contains(std::to_string(kMapRegion))) {
      for (const auto& kXteaKey :
           kCacheConfig["map_archive_keys"][std::to_string(kMapRegion)]
               .get<std::vector<uint32_t>>()) {
        buffer.PutDataBE<uint32_t>(kXteaKey);
      }
    } else {
      buffer.PutDataBE<uint32_t>(0);
      buffer.PutDataBE<uint32_t>(0);
      buffer.PutDataBE<uint32_t>(0);
      buffer.PutDataBE<uint32_t>(0);
    }
  }

  openrs::net::codec::Packet region_packet;
  region_packet.type = openrs::net::codec::PacketType::kMapRegion;
  region_packet.data = buffer;
  session->Send(region_packet);
}

void openrs::manager::WorldManager::SendDynamicMapRegion(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    const std::shared_ptr<openrs::net::Session>& session,
    const bool kSendLocalPlayerUpdate) const {
  openrs::common::io::BitBuffer<> buffer;
  if (kSendLocalPlayerUpdate) {
    this->GetLocalPlayerUpdate(1, session->player_index(), kPlayer, &buffer);
  }

  buffer.PutShiftedNegDataBE<uint8_t>(2);
  buffer.PutDataLE<uint16_t>(kPlayer->chunk_y());
  buffer.PutShiftedPosDataLE<uint16_t>(kPlayer->chunk_x());
  buffer.PutShiftedNegDataBE<uint8_t>(
      kPlayer->force_next_map_load_refresh() ? 1 : 0);
  buffer.PutData<uint8_t>(-1 * kPlayer->map_size());

  std::vector<uint32_t> map_regions;
  std::set<uint32_t> real_map_regions;
  kPlayer->GetDynamicMapRegions(
      openrs::game::WorldTile::MAP_SIZES[kPlayer->map_size()] >> 4,
      &map_regions, &real_map_regions);
  auto bitset = new std::vector<bool>();
  for (const auto& kMapRegion : map_regions) {
    if (kMapRegion == 0) {
      bitset->emplace_back(false);
    } else {
      const auto kRegionHash = std::bitset<18>(kMapRegion);
      bitset->emplace_back(true);
      for (size_t i = 0; i < kRegionHash.size(); ++i) {
        bitset->emplace_back(kRegionHash[i]);
      }
    }
  }

  buffer.PutBitSetBE(*bitset);
  delete bitset;

  const auto& kCacheConfig =
      openrs::manager::ConfigManager::get()->cache_config().at(718);
  for (const auto& kMapRegion : real_map_regions) {
    if (kCacheConfig["map_archive_keys"].contains(std::to_string(kMapRegion))) {
      for (const auto& kXteaKey :
           kCacheConfig["map_archive_keys"][std::to_string(kMapRegion)]
               .get<std::vector<uint32_t>>()) {
        buffer.PutDataBE<uint32_t>(kXteaKey);
      }
    } else {
      buffer.PutDataBE<uint32_t>(0);
      buffer.PutDataBE<uint32_t>(0);
      buffer.PutDataBE<uint32_t>(0);
      buffer.PutDataBE<uint32_t>(0);
    }
  }

  openrs::net::codec::Packet region_packet;
  region_packet.type = openrs::net::codec::PacketType::kDynamicMapRegion;
  region_packet.data = buffer;
  session->Send(region_packet);
}

void openrs::manager::WorldManager::SendLocalPlayerUpdate(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    const std::shared_ptr<openrs::net::Session>& session) const {
  openrs::common::io::BitBuffer<> buffer;

  openrs::common::io::Buffer<> appearance_buffer;
  for (const auto& kWorld : this->worlds_) {
    for (const auto& kLocalPlayer : kWorld.second.players()) {
      const uint32_t kDataMask = 0x10;
      appearance_buffer.PutData(static_cast<uint8_t>(kDataMask));
      if (kDataMask > std::numeric_limits<uint8_t>::max()) {
        appearance_buffer.PutData(static_cast<uint8_t>(kDataMask >> 8));
      }
      if (kDataMask > std::numeric_limits<uint16_t>::max()) {
        appearance_buffer.PutData(static_cast<uint8_t>(kDataMask >> 16));
      }

      openrs::manager::AppearanceManager::GetPlayerAppearance(
          kLocalPlayer.second, &appearance_buffer);

      // Clan.
      appearance_buffer.PutData<uint8_t>(0);

      buffer.PutData<uint8_t>(1, 1);
      buffer.PutData<uint8_t>(1, 1);
      buffer.PutData<uint8_t>(2, 0);
    }
  }

  buffer.insert(buffer.end(), appearance_buffer.begin(),
                appearance_buffer.end());

  openrs::net::codec::Packet packet;
  packet.type = openrs::net::codec::PacketType::kLocalPlayerUpdate;
  packet.data = buffer;
  session->Send(packet);
}

void openrs::manager::WorldManager::SendRunEnergy(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    const std::shared_ptr<openrs::net::Session>& session) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutData<uint8_t>(kPlayer->run_energy());

  openrs::net::codec::Packet energy_packet;
  energy_packet.type = openrs::net::codec::PacketType::kRunEnergy;
  energy_packet.data = buffer;
  session->Send(energy_packet);
}

void openrs::manager::WorldManager::SendPlayerHitPoints(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    const std::shared_ptr<openrs::net::Session>& session) const {
  openrs::manager::ConfigManager::get()->SendFileConfig(
      kPlayer, session, openrs::manager::ConfigManager::ConfigFile::kHitPoints,
      kPlayer->hit_points());
}

void openrs::manager::WorldManager::SendItemLook(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    const std::shared_ptr<openrs::net::Session>& session) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutData<uint8_t>(kPlayer->old_items_look() ? 1 : 0);

  openrs::net::codec::Packet packet;
  packet.type = openrs::net::codec::PacketType::kItemLook;
  packet.data = buffer;
  session->Send(packet);
}

void openrs::manager::WorldManager::SendCustom161(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    const std::shared_ptr<openrs::net::Session>& session) const {
  openrs::common::io::Buffer<> buffer;
  uint8_t flag = 0;
  if (kPlayer->is_shift_drop()) {
    flag |= 1;
  }
  if (kPlayer->is_slow_drag()) {
    flag |= 2;
  }
  if (kPlayer->is_zoom()) {
    flag |= 4;
  }

  buffer.PutData<uint8_t>(flag);

  openrs::net::codec::Packet packet;
  packet.type = openrs::net::codec::PacketType::kCustomPacket161;
  packet.data = buffer;
  session->Send(packet);
}

void openrs::manager::WorldManager::SendMessage(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    const std::shared_ptr<openrs::net::Session>& session,
    const MessageType& kMessageType, const std::string& kMessage) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutSmartBE(static_cast<uint16_t>(kMessageType));
  buffer.PutDataBE<uint32_t>(kPlayer->tile_hash());
  buffer.PutDataBE<uint8_t>(1);
  buffer.PutString(kPlayer->username);
  buffer.PutString(kMessage);

  openrs::net::codec::Packet packet;
  packet.type = openrs::net::codec::PacketType::kMessage;
  packet.data = buffer;
  session->Send(packet);
}

void openrs::manager::WorldManager::SendCreateWorldTile(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    const std::shared_ptr<openrs::net::Session>& session,
    const openrs::game::WorldTile& kTile) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutShiftedPosDataBE<uint8_t>(kTile.GetLocalY(
      *kPlayer, openrs::game::WorldTile::MAP_SIZES[kPlayer->map_size()] >> 4));
  buffer.PutDataBE<uint8_t>(-1 * kTile.plane());
  buffer.PutShiftedNegDataBE<uint8_t>(kTile.GetLocalX(
      *kPlayer, openrs::game::WorldTile::MAP_SIZES[kPlayer->map_size()] >> 4));

  openrs::net::codec::Packet packet;
  packet.type = openrs::net::codec::PacketType::kCreateTile;
  packet.data = buffer;
  session->Send(packet);
}

void openrs::manager::WorldManager::SendMusic(
    const std::shared_ptr<openrs::game::Player>& player,
    const std::shared_ptr<openrs::net::Session>& session,
    const MusicTrackID& kMusicId, const uint8_t& kDelay,
    const uint8_t& kVolume) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutDataBE<uint8_t>(kDelay);
  buffer.PutShiftedPosDataLE(static_cast<uint16_t>(kMusicId));
  buffer.PutShiftedPosDataLE(kVolume);

  openrs::net::codec::Packet packet;
  packet.type = openrs::net::codec::PacketType::kMusic;
  packet.data = buffer;
  session->Send(packet);
}

void openrs::manager::WorldManager::SendMusicEffect(
    const std::shared_ptr<openrs::game::Player>& player,
    const std::shared_ptr<openrs::net::Session>& session,
    const MusicEffectID& kMusicId, const uint8_t& kVolume) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutShiftedNegDataLE(kVolume);
  buffer.PutData<uint8_t>(0);
  buffer.PutData<uint8_t>(0);
  buffer.PutData<uint8_t>(0);
  buffer.PutDataBE(static_cast<uint16_t>(kMusicId));

  openrs::net::codec::Packet packet;
  packet.type = openrs::net::codec::PacketType::kMusicEffect;
  packet.data = buffer;
  session->Send(packet);
}

void openrs::manager::WorldManager::SendMultiCombatArea(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    const std::shared_ptr<openrs::net::Session>& session) const {
  const auto& kConfigManager = openrs::manager::ConfigManager::get();
  if (this->worlds().at(1).IsMultiCombatArea(*kPlayer)) {
    kConfigManager->SendGlobalConfig(
        kPlayer, session,
        openrs::manager::ConfigManager::ConfigGlobal::kCombatMode, 1);
  } else {
    kConfigManager->SendGlobalConfig(
        kPlayer, session,
        openrs::manager::ConfigManager::ConfigGlobal::kCombatMode, 0);
  }
}

void openrs::manager::WorldManager::SendPlayerSkill(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    const std::shared_ptr<openrs::net::Session>& session,
    const openrs::game::player::Skills::Skill& kSkill) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutShiftedNegDataBE(
      static_cast<std::underlying_type_t<openrs::game::player::Skills::Skill>>(
          kSkill));
  buffer.PutDataBE<uint32_t>(kPlayer->GetSkillExperience(kSkill));
  buffer.PutShiftedPosDataBE(kPlayer->GetSkillLevel(kSkill));

  openrs::net::codec::Packet packet;
  packet.type = openrs::net::codec::PacketType::kPlayerSkill;
  packet.data = buffer;
  session->Send(packet);
}
