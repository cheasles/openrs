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
#include "openrs/game/player.h"

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
