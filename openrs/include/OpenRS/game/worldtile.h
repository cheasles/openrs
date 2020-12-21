#pragma once

#include <inttypes.h>

namespace openrs {
namespace game {

class WorldTile {
 private:
  uint16_t x_;
  uint16_t y_;
  uint8_t z_;

 public:
  WorldTile() : x_(0), y_(0), z_(0) {}
  WorldTile(const WorldTile& other)
      : x_(other.x_), y_(other.y_), z_(other.z_) {}
  WorldTile(const uint16_t& x, const uint16_t& y, const uint8_t& z)
      : x_(x), y_(y), z_(z) {}
  WorldTile& operator=(const WorldTile& other) {
    this->x_ = other.x_;
    this->y_ = other.y_;
    this->z_ = other.z_;
  }

  inline auto x() const { return this->x_; }
  inline auto chunk_x() const { return this->x_ >> 3; }
  inline auto region_x() const { return this->x_ >> 6; }
  inline auto y() const { return this->y_; }
  inline auto chunk_y() const { return this->y_ >> 3; }
  inline auto region_y() const { return this->y_ >> 6; }
  inline auto z() const { return this->z_; }
  inline auto plane() const { return this->z_; }
  inline auto region_id() const {
    return (this->region_x() << 8) + this->region_y();
  }

  inline void set_x(const uint16_t& x) { this->x_ = x; }
  inline void set_y(const uint16_t& y) { this->y_ = y; }
  inline void set_z(const uint8_t& z) { this->z_ = z; }
  inline void set_plane(const uint8_t& plane) { this->z_ = plane; }
};

}  // namespace game
}  // namespace openrs
