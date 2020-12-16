#include "OpenRS/database/models/player.h"

#include <base64.h>
#include <filters.h>
#include <pwdbased.h>
#include <sha.h>

bool openrs::database::models::PlayerModel::CheckPassword(
    const std::vector<uint8_t>& kPassword, const uint32_t& kIterations) const {
  std::vector<uint8_t> encoded_password;
  encoded_password.resize(32);

  std::vector<uint8_t> salt;
  CryptoPP::StringSource(
      this->salt, true,
      new CryptoPP::Base64Decoder(new CryptoPP::VectorSink(salt)));
  std::vector<uint8_t> password;
  CryptoPP::StringSource(
      this->password, true,
      new CryptoPP::Base64Decoder(new CryptoPP::VectorSink(password)));

  CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> pbkdf2;
  pbkdf2.DeriveKey(encoded_password.data(), encoded_password.size(), 0,
                   kPassword.data(), kPassword.size(), salt.data(), salt.size(),
                   kIterations);
  return std::equal(encoded_password.cbegin(), encoded_password.cend(),
                    kPassword.cbegin(), kPassword.cend());
}
