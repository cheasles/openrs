#pragma once

#include <base64.h>
#include <filters.h>
#include <osrng.h>

#include "OpenRS/database/models/player.h"

namespace openrs {
namespace game {

class Player : public database::models::PlayerModel {
 public:
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

  inline bool CheckPassword(const std::string& kPassword,
                            const uint32_t& kIterations = 10) const {
    std::string encoded_password;
    this->EncodePassword(kPassword, this->salt, &encoded_password, kIterations);
    return std::equal(encoded_password.cbegin(), encoded_password.cend(),
                      this->password.cbegin(), this->password.cend());
  }

  static inline void GenerateRandomString(const uint32_t& kLength,
                                          std::string* output) {
    CryptoPP::SecByteBlock random(kLength);
    CryptoPP::OS_GenerateRandomBlock(true, random, random.size());
    CryptoPP::ArraySource(
        random.data(), random.size(), true,
        new CryptoPP::Base64Encoder(new CryptoPP::StringSink(*output), false));
  }

  Player& operator=(const Player&) = default;
  inline Player& operator=(const database::models::PlayerModel& other) {
    this->id = other.id;
    this->username = other.username;
    this->password = other.password;
    this->salt = other.salt;
    return *this;
  }
};

}  // namespace game
}  // namespace openrs
