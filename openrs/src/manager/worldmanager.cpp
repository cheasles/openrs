#include "openrs/manager/worldmanager.h"

#include <openrs/common/io/buffer.h>
#include <openrs/common/log.h>

#include <bitset>

#include "openrs/manager/appearancemanager.h"
#include "openrs/manager/configmanager.h"
#include "openrs/manager/interfacemanager.h"

bool openrs::manager::WorldManager::Init() {
  this->add_world(1, openrs::game::World());

  common::Log(common::Log::LogLevel::kInfo)
      << "[WorldManager] Initialized " << this->worlds_.size() << " worlds.";
  return true;
}

void openrs::manager::WorldManager::StartPlayer(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    openrs::net::Session* session) const {
  const auto& kInterfaceManager = openrs::manager::InterfaceManager::get();
  const auto& kConfigManager = openrs::manager::ConfigManager::get();

  this->SendMap(kPlayer, session, true);
  kInterfaceManager.SendInterfaces(kPlayer, session);
  this->SendRunEnergy(kPlayer, session);
  this->SendPlayerHitPoints(kPlayer, session);
  this->SendItemLook(kPlayer, session);
  this->SendCustom161(kPlayer, session);
  this->SendMessage(
      kPlayer, session, openrs::manager::WorldManager::MessageType::kDefault,
      "Welcome to " + kConfigManager["server"]["name"].get<std::string>());
  this->SendMessage(
      kPlayer, session, openrs::manager::WorldManager::MessageType::kDefault,
      kConfigManager["server"]["name"].get<std::string>() +
          " is distributed under the GNU AGPL license. Please visit " +
          kConfigManager["server"]["source_url"].get<std::string>() +
          " to view the source code.");

  this->SendMultiCombatArea(kPlayer, session);
  kConfigManager.SendConfig(kPlayer, session,
                            openrs::manager::ConfigManager::Config::k281, 1000);
  kConfigManager.SendConfig(kPlayer, session,
                            openrs::manager::ConfigManager::Config::k1159, 1);
  kConfigManager.SendConfig(kPlayer, session,
                            openrs::manager::ConfigManager::Config::k1160, -1);
  kConfigManager.SendGameBarStages(kPlayer, session);

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
    openrs::net::Session* session, const bool kSendLocalPlayerUpdate) const {
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
      openrs::manager::ConfigManager::get().cache_config().at(718);
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
    openrs::net::Session* session, const bool kSendLocalPlayerUpdate) const {
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
      openrs::manager::ConfigManager::get().cache_config().at(718);
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
    openrs::net::Session* session) const {
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
    openrs::net::Session* session) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutData<uint8_t>(kPlayer->run_energy());

  openrs::net::codec::Packet energy_packet;
  energy_packet.type = openrs::net::codec::PacketType::kRunEnergy;
  energy_packet.data = buffer;
  session->Send(energy_packet);
}

void openrs::manager::WorldManager::SendPlayerHitPoints(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    openrs::net::Session* session) const {
  openrs::manager::ConfigManager::get().SendFileConfig(
      kPlayer, session, openrs::manager::ConfigManager::ConfigFile::kHitPoints,
      kPlayer->hit_points());
}

void openrs::manager::WorldManager::SendItemLook(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    openrs::net::Session* session) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutData<uint8_t>(kPlayer->old_items_look() ? 1 : 0);

  openrs::net::codec::Packet packet;
  packet.type = openrs::net::codec::PacketType::kItemLook;
  packet.data = buffer;
  session->Send(packet);
}

void openrs::manager::WorldManager::SendCustom161(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    openrs::net::Session* session) const {
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
    openrs::net::Session* session, const MessageType& kMessageType,
    const std::string& kMessage) const {
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
    openrs::net::Session* session, const openrs::game::WorldTile& kTile) const {
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
    openrs::net::Session* session, const MusicTrackID& kMusicId,
    const uint8_t& kDelay, const uint8_t& kVolume) const {
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
    openrs::net::Session* session, const MusicEffectID& kMusicId,
    const uint8_t& kVolume) const {
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
    openrs::net::Session* session) const {
  const auto& kConfigManager = openrs::manager::ConfigManager::get();
  if (this->worlds().at(1).IsMultiCombatArea(*kPlayer)) {
    kConfigManager.SendGlobalConfig(
        kPlayer, session,
        openrs::manager::ConfigManager::ConfigGlobal::kCombatMode, 1);
  } else {
    kConfigManager.SendGlobalConfig(
        kPlayer, session,
        openrs::manager::ConfigManager::ConfigGlobal::kCombatMode, 0);
  }
}

void openrs::manager::WorldManager::SendPlayerSkill(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    openrs::net::Session* session,
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
