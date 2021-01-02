#include "openrs/manager/worldmanager.h"

#include <openrs/common/log.h>

#include <bitset>

#include "openrs/manager/configmanager.h"

bool openrs::manager::WorldManager::Init() {
  this->add_world(1, openrs::game::World());

  common::Log(common::Log::LogLevel::kInfo)
      << "[WorldManager] Initialized " << this->worlds_.size() << " worlds.";
  return true;
}

void openrs::manager::WorldManager::GetLocalPlayerUpdate(
    const uint32_t& kWorldId, const uint32_t& kPlayerIndex,
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    openrs::common::io::Buffer<>* buffer) const {
  auto bitset = new std::bitset<30 + 2046 * 18>();
  size_t bitset_offset = 0;
  const auto kTileHash = std::bitset<30>(kPlayer->tile_hash());
  for (size_t i = 0; i < kTileHash.size(); ++i) {
    (*bitset)[bitset_offset++] = kTileHash[i];
  }
  for (const auto& kLocalPlayer : this->worlds_.at(kWorldId).players()) {
    if (kLocalPlayer.first == kPlayerIndex) {
      continue;
    }

    bitset_offset = 30 + 18 * kLocalPlayer.first;
    const auto kRegionHash =
        std::bitset<18>(kLocalPlayer.second->region_hash());
    for (size_t i = 0; i < kRegionHash.size(); ++i) {
      (*bitset)[bitset_offset++] = kRegionHash[i];
    }
  }

  buffer->PutBitSetBE<30 + 2046 * 18>(*bitset);
  delete bitset;
}

void openrs::manager::WorldManager::SendMapRegion(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    openrs::net::Session* session, const bool kSendLocalPlayerUpdate) const {
  openrs::common::io::Buffer<> buffer;
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
  openrs::common::io::Buffer<> buffer;
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
      kPlayer, session, openrs::manager::ConfigManager::FileConfig::kHitPoints,
      kPlayer->hit_points());
}

void openrs::manager::WorldManager::SendPlayerOption(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    openrs::net::Session* session, const std::string kOption,
    const uint8_t& kSlot, const bool& kTop, const uint16_t& kCursor) const {
  openrs::common::io::Buffer<> buffer;
  buffer.PutShiftedPosDataBE(kSlot);
  buffer.PutString(kOption);
  buffer.PutShiftedPosDataLE(kCursor);
  buffer.PutData<uint8_t>(-1 * (kTop ? 1 : 0));

  openrs::net::codec::Packet packet;
  packet.type = openrs::net::codec::PacketType::kPlayerOption;
  packet.data = buffer;
  session->Send(packet);
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
