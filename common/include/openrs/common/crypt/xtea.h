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
