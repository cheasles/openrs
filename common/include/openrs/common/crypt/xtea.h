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

#include <filters.h>
#include <inttypes.h>
#include <modes.h>
#include <tea.h>

namespace openrs {
namespace common {
namespace crypt {

inline void EncryptXTEA(const uint8_t *kKeys, const uint32_t &kKeysSize,
                        const uint8_t *kInput, const uint32_t &kInputSize,
                        std::vector<uint8_t> *output) {
  CryptoPP::ECB_Mode<CryptoPP::XTEA>::Encryption e;
  e.SetKey(kKeys, kKeysSize);
  CryptoPP::ArraySource(
      kInput, kInputSize, true,
      new CryptoPP::StreamTransformationFilter(
          e, new CryptoPP::VectorSink(*output),
          CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::NO_PADDING));
}

inline void DecryptXTEA(const uint8_t *kKeys, const uint32_t &kKeysSize,
                        const uint8_t *kInput, const uint32_t &kInputSize,
                        std::vector<uint8_t> *output) {
  CryptoPP::ECB_Mode<CryptoPP::XTEA>::Decryption d;
  d.SetKey(kKeys, kKeysSize);
  CryptoPP::ArraySource(
      kInput, kInputSize, true,
      new CryptoPP::StreamTransformationFilter(
          d, new CryptoPP::VectorSink(*output),
          CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::NO_PADDING));
}

}  // namespace crypt
}  // namespace common
}  // namespace openrs
