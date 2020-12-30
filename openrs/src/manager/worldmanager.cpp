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
    const uint32_t& kWorldId,
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    openrs::net::Session* session, openrs::common::io::Buffer<>* buffer) const {
  auto bitset = new std::bitset<30 + 2046 * 18>();
  size_t bitset_offset = 0;
  const auto kTileHash = std::bitset<30>(kPlayer->tile_hash());
  for (size_t i = 0; i < kTileHash.size(); ++i) {
    (*bitset)[bitset_offset++] = kTileHash[i];
  }
  for (const auto& kLocalPlayer : this->worlds_.at(kWorldId).players()) {
    if (kLocalPlayer.first == session->player_index()) {
      continue;
    }

    bitset_offset = 30 + 18 * kLocalPlayer.first;
    const auto kRegionHash = std::bitset<18>(kPlayer->region_hash());
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
    this->GetLocalPlayerUpdate(1, kPlayer, session, &buffer);
  }
  buffer.PutData<uint8_t>(-1 * kPlayer->map_size());
  buffer.PutData<uint8_t>(0);
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
