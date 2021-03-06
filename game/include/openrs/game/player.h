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

#include <base64.h>
#include <filters.h>
#include <openrs/database/models/player.h>
#include <osrng.h>

#include "openrs/game/entity.h"
#include "openrs/game/graphics.h"
#include "openrs/game/player/appearance.h"
#include "openrs/game/player/skills.h"

namespace openrs {
namespace game {

class Player : public database::models::PlayerModel,
               public openrs::game::Entity,
               public openrs::game::player::Appearance,
               public openrs::game::player::Skills {
 public:
  enum struct DisplayMode : uint8_t {
    kUnknown = 0,
    kUnknown1 = 1,
    kResizable = 2,
    kUnknown3 = 3
  };

 private:
  DisplayMode display_mode_;
  uint16_t screen_width_;
  uint16_t screen_height_;
  bool force_next_map_load_refresh_;
  uint8_t run_energy_;
  uint32_t hit_points_;
  bool old_items_look_;
  bool is_shift_drop_;
  bool is_slow_drag_;
  bool is_zoom_;
  bool update_movement_type_;
  bool is_running_;

 public:
  Player()
      : display_mode_(DisplayMode::kUnknown),
        screen_width_(0),
        screen_height_(0),
        force_next_map_load_refresh_(false),
        run_energy_(0),
        hit_points_(100),
        old_items_look_(false),
        is_shift_drop_(false),
        is_slow_drag_(false),
        is_zoom_(false),
        update_movement_type_(true),
        is_running_(true),
        database::models::PlayerModel() {
    this->SetNextGraphics(openrs::game::Graphics(188));
  }

  Player& operator=(const Player& kOther) {
    database::models::PlayerModel::operator=(kOther);
    openrs::game::Entity::operator=(kOther);
    openrs::game::player::Appearance::operator=(kOther);
    openrs::game::player::Skills::operator=(kOther);
    return *this;
  }
  inline Player& operator=(const database::models::PlayerModel& kOther) {
    database::models::PlayerModel::operator=(kOther);
    return *this;
  }

  static inline void EncodePassword(const std::string& kPassword,
                                    const std::string& kSalt,
                                    std::string* output,
                                    const uint32_t& kIterations = 10) {
    std::vector<uint8_t> password;
    CryptoPP::StringSource(kPassword, true, new CryptoPP::VectorSink(password));
    std::vector<uint8_t> salt;
    CryptoPP::StringSource(kSalt, true, new CryptoPP::VectorSink(salt));
    Player::EncodePassword(password, salt, output, kIterations);
  }

  static void EncodePassword(const std::vector<uint8_t>& kPassword,
                             const std::vector<uint8_t>& kSalt,
                             std::string* output,
                             const uint32_t& kIterations = 10);

  static inline void GenerateRandomString(const uint32_t& kLength,
                                          std::string* output) {
    CryptoPP::SecByteBlock random(kLength);
    CryptoPP::OS_GenerateRandomBlock(true, random, random.size());
    CryptoPP::ArraySource(
        random.data(), random.size(), true,
        new CryptoPP::Base64Encoder(new CryptoPP::StringSink(*output), false));
  }

  inline bool CheckPassword(const std::string& kPassword,
                            const uint32_t& kIterations = 10) const {
    std::string encoded_password;
    this->EncodePassword(kPassword, this->salt, &encoded_password, kIterations);
    return std::equal(encoded_password.cbegin(), encoded_password.cend(),
                      this->password.cbegin(), this->password.cend());
  }

  /**
   * Checks if the player has a skill or not.
   *
   * @return True if skulled, false otherwise.
   */
  inline bool HasSkull() const { return this->skull_time_left > 0; }

  inline auto display_mode() const { return this->display_mode_; }
  inline auto screen_width() const { return this->screen_width_; }
  inline auto screen_height() const { return this->screen_height_; }
  inline auto force_next_map_load_refresh() const {
    return this->force_next_map_load_refresh_;
  }
  inline auto run_energy() const { return this->run_energy_; }
  inline auto hit_points() const { return this->hit_points_; }
  inline auto old_items_look() const { return this->old_items_look_; }
  inline auto is_shift_drop() const { return this->is_shift_drop_; }
  inline auto is_slow_drag() const { return this->is_slow_drag_; }
  inline auto is_zoom() const { return this->is_zoom_; }
  inline auto update_movement_type() const {
    return this->update_movement_type_;
  }
  inline auto is_running() const { return this->is_running_; }

  inline void set_display_mode(const DisplayMode& kDisplayMode) {
    this->display_mode_ = kDisplayMode;
  }
  inline void set_screen_width(const uint16_t& kScreenWidth) {
    this->screen_width_ = kScreenWidth;
  }
  inline void set_screen_height(const uint16_t& kScreenHeight) {
    this->screen_height_ = kScreenHeight;
  }
  inline void set_force_next_map_load_refresh(
      const bool& kSkForceNextMapLoadRefresh) {
    this->force_next_map_load_refresh_ = kSkForceNextMapLoadRefresh;
  }
  inline void set_run_energy(const uint8_t& kRunEnergy) {
    this->run_energy_ = kRunEnergy;
  }
  inline void set_hit_points(const uint8_t& kHitPoints) {
    this->hit_points_ = kHitPoints;
  }
  inline void set_old_items_look(const bool& kOldItemsLook) {
    this->old_items_look_ = kOldItemsLook;
  }
  inline void set_update_movement_type(const bool& kUpdateMovementType) {
    this->update_movement_type_ = kUpdateMovementType;
  }
  inline void set_is_running(const bool& kIsRunning) {
    this->is_running_ = kIsRunning;
  }
};

}  // namespace game
}  // namespace openrs
