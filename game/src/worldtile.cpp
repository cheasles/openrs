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

void openrs::game::WorldTile::GetDynamicMapRegions(
    const uint8_t& kMapSize, std::vector<uint32_t>* output_client,
    std::set<uint32_t>* output_real) const {
  const auto& kChunkX = this->chunk_x();
  const auto& kChunkY = this->chunk_y();
  const auto& kMinRegionX = kChunkX < kMapSize ? 0 : (kChunkX - kMapSize);
  const auto& kMinRegionY = kChunkY < kMapSize ? 0 : (kChunkY - kMapSize);
  const auto& kMaxRegionX = (kChunkX + kMapSize);
  const auto& kMaxRegionY = (kChunkY + kMapSize);
  output_client->reserve(4 * (kMaxRegionX - kMinRegionX) *
                  (kMaxRegionY - kMinRegionY));

  for (uint8_t plane = 0; plane < 4; ++plane) {
    for (auto x = kMinRegionX; x <= kMaxRegionX; ++x) {
      for (auto y = kMinRegionY; y <= kMaxRegionY; ++y) {
        if (x == 0 || y == 0) {
          // Invalid region.
          output_client->emplace_back(0);
        } else {
          const auto kRotation = 0;
          const auto kRealRegionId = y / 8 + ((x / 8) << 8);
          const auto kClientRegionId =
              (kRotation << 1) | (plane << 24) | (x << 14) | (y << 3);
          output_client->emplace_back(kClientRegionId);
          output_real->emplace(kRealRegionId);
        }
      }
    }
  }
}
