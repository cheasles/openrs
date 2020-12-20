#include "OpenRS/game/player.h"

#include <base64.h>
#include <filters.h>
#include <pwdbased.h>
#include <sha.h>

void openrs::game::Player::EncodePassword(const std::vector<uint8_t>& kPassword,
                                          const std::vector<uint8_t>& kSalt,
                                          std::string* output,
                                          const uint32_t& kIterations) {
  std::vector<uint8_t> encoded_password;
  encoded_password.resize(32);

  CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> pbkdf2;
  pbkdf2.DeriveKey(encoded_password.data(), encoded_password.size(), 0,
                   kPassword.data(), kPassword.size(), kSalt.data(),
                   kSalt.size(), kIterations);
  CryptoPP::VectorSource(
      encoded_password, true,
      new CryptoPP::Base64Encoder(new CryptoPP::StringSink(*output), false));
}
