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
#include <openrs/database/columnsets/worldtile.h>

#include <array>
#include <set>
#include <vector>

namespace openrs {
namespace game {

class WorldTile
    : virtual public openrs::database::columnsets::WorldTileColumnSet {
 public:
  static inline constexpr std::array<uint16_t, 5> MAP_SIZES{104, 120, 136, 168,
                                                            72};

 private:
  uint8_t map_size_;

 public:
  WorldTile()
      : openrs::database::columnsets::WorldTileColumnSet(), map_size_(0) {}
  WorldTile(const WorldTile& other)
      : openrs::database::columnsets::WorldTileColumnSet(other),
        map_size_(other.map_size_) {}
  WorldTile(const uint16_t& x, const uint16_t& y, const uint8_t& z)
      : openrs::database::columnsets::WorldTileColumnSet(x, y, z),
        map_size_(0) {}
  WorldTile& operator=(const WorldTile& other) {
    this->position_x = other.position_x;
    this->position_y = other.position_y;
    this->position_z = other.position_z;
    this->map_size_ = other.map_size_;
    return *this;
  }

  /**
   * Calculates all fixed map region IDs for the current tile.
   *
   * @param kMapSize The number of surrounding tiles to include.
   * @param output Output storage for regions.
   */
  void GetMapRegions(const uint8_t& kMapSize,
                     std::vector<uint32_t>* output) const;

  /**
   * Calculates all dynamic map region IDs for the current tile.
   *
   * @param kMapSize The number of surrounding tiles to include.
   * @param output Output storage for regions as a list of pairs of region IDs,
   *  the first being the client ID and the second the 'real' one.
   */
  void GetDynamicMapRegions(const uint8_t& kMapSize,
                            std::vector<uint32_t>* output_client,
                            std::set<uint32_t>* output_real) const;

  inline bool WithinDistance(const WorldTile& kPoint,
                             const uint32_t& kDistance = 14) const {
    if (this->z() != kPoint.z()) {
      return false;
    }

    const int32_t kDeltaX = kPoint.x() - this->x();
    const int32_t kDeltaY = kPoint.y() - this->y();
    return kDeltaX <= kDistance && kDeltaX >= -1 * kDistance &&
           kDeltaY <= kDistance && kDeltaY >= -1 * kDistance;
  }

  inline uint32_t GetLocalX(const WorldTile& local, const uint8_t& kMapSize) const {
    return this->x() - 8 * (local.chunk_x() - kMapSize);
  }

  inline uint32_t GetLocalY(const WorldTile& local, const uint8_t& kMapSize) const {
    return this->y() - 8 * (local.chunk_y() - kMapSize);
  }

  inline uint32_t x() const { return this->position_x; }
  inline uint32_t chunk_x() const { return this->position_x >> 3; }
  inline uint32_t region_x() const { return this->position_x >> 6; }
  inline uint32_t y() const { return this->position_y; }
  inline uint32_t chunk_y() const { return this->position_y >> 3; }
  inline uint32_t region_y() const { return this->position_y >> 6; }
  inline uint32_t z() const { return this->position_z; }
  inline uint32_t plane() const { return this->position_z; }
  inline uint32_t region_id() const {
    return (this->region_x() << 8) + this->region_y();
  }
  inline uint32_t region_hash() const {
    return this->region_y() + (this->region_x() << 8) + (this->z() << 16);
  }
  inline uint32_t tile_hash() const {
    return this->y() + (this->x() << 14) + (this->z() << 28);
  }
  inline auto map_size() const { return this->map_size_; }

  inline void set_x(const uint16_t& x) { this->position_x = x; }
  inline void set_y(const uint16_t& y) { this->position_y = y; }
  inline void set_z(const uint8_t& z) { this->position_z = z; }
  inline void set_plane(const uint8_t& plane) { this->position_z = plane; }
};

}  // namespace game
}  // namespace openrs
