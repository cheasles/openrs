#pragma once

#include <array>
#include <inttypes.h>
#include <openrs/database/columnsets/worldtile.h>

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
      : openrs::database::columnsets::WorldTileColumnSet(x, y, z), map_size_(0) {}
  WorldTile& operator=(const WorldTile& other) {
    this->position_x = other.position_x;
    this->position_y = other.position_y;
    this->position_z = other.position_z;
    this->map_size_ = other.map_size_;
    return *this;
  }

  void GetMapRegions(const uint8_t& kMapSize,
                     std::vector<uint32_t>* output) const;

  inline auto x() const { return this->position_x; }
  inline auto chunk_x() const { return this->position_x >> 3; }
  inline auto region_x() const { return this->position_x >> 6; }
  inline auto y() const { return this->position_y; }
  inline auto chunk_y() const { return this->position_y >> 3; }
  inline auto region_y() const { return this->position_y >> 6; }
  inline auto z() const { return this->position_z; }
  inline auto plane() const { return this->position_z; }
  inline auto region_id() const {
    return (this->region_x() << 8) + this->region_y();
  }
  inline auto map_size() const { return this->map_size_; }

  inline void set_x(const uint16_t& x) { this->position_x = x; }
  inline void set_y(const uint16_t& y) { this->position_y = y; }
  inline void set_z(const uint8_t& z) { this->position_z = z; }
  inline void set_plane(const uint8_t& plane) { this->position_z = plane; }
};

}  // namespace game
}  // namespace openrs
