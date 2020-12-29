#include "openrs/game/worldtile.h"

void openrs::game::WorldTile::GetMapRegions(
    const uint8_t& kMapSize, std::vector<uint32_t>* output) const {
  const auto& kChunkX = this->chunk_x();
  const auto& kChunkY = this->chunk_y();
  const auto& kMinRegionX = kChunkX < kMapSize ? 0 : (kChunkX - kMapSize) / 8;
  const auto& kMinRegionY = kChunkY < kMapSize ? 0 : (kChunkY - kMapSize) / 8;
  const auto& kMaxRegionX = (kChunkX + kMapSize) / 8;
  const auto& kMaxRegionY = (kChunkY + kMapSize) / 8;
  output->reserve((kMaxRegionX - kMinRegionX) * (kMaxRegionY - kMinRegionY));
  for (auto x = kMinRegionX; x <= kMaxRegionX; ++x) {
    for (auto y = kMinRegionY; y <= kMaxRegionY; ++y) {
      const auto kRegionId = y + (x << 8);
      output->emplace_back(kRegionId);
    }
  }
}
