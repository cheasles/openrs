#pragma once

#include <base64.h>
#include <filters.h>
#include <openrs/database/models/player.h>
#include <osrng.h>

namespace openrs {
namespace game {

class Player : public database::models::PlayerModel {
 public:
  enum struct DisplayMode : uint8_t {
    kUnknown = 0,
    kUnknown1 = 1,
    kUnknown2 = 2,
    kUnknown3 = 3
  };

 private:
  DisplayMode display_mode_;
  uint16_t screen_width_;
  uint16_t screen_height_;

 public:
  Player()
      : display_mode_(DisplayMode::kUnknown),
        screen_width_(0),
        screen_height_(0),
        database::models::PlayerModel() {}

  Player& operator=(const Player&) = default;
  inline Player& operator=(const database::models::PlayerModel& other) {
    this->id = other.id;
    this->username = other.username;
    this->password = other.password;
    this->salt = other.salt;
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

  inline auto display_mode() const { return this->display_mode_; }
  inline auto screen_width() const { return this->screen_width_; }
  inline auto screen_height() const { return this->screen_height_; }

  inline void set_display_mode(const DisplayMode& kDisplayMode) {
    this->display_mode_ = kDisplayMode;
  }
  inline void set_screen_width(const uint16_t& kScreenWidth) {
    this->screen_width_ = kScreenWidth;
  }
  inline void set_screen_height(const uint16_t& kScreenHeight) {
    this->screen_height_ = kScreenHeight;
  }
};

}  // namespace game
}  // namespace openrs
