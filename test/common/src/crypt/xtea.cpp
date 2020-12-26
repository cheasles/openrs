#include <gtest/gtest.h>
#include <openrs/common/crypt/xtea.h>

#include <algorithm>

TEST(XTEA, Decrypt) {
  const std::vector<uint8_t> encrypted = {
      0x65, 0x57, 0xa0, 0x4c, 0xf3, 0xa6, 0xb7, 0xfd, 0xba, 0xa3, 0xb6, 0xf2,
      0x29, 0x84, 0x58, 0x63, 0x65, 0xa8, 0xc8, 0x8b, 0xe1, 0x26, 0x75, 0x37,
      0x65, 0xa8, 0xc8, 0x8b, 0xe1, 0x26, 0x75, 0x37, 0xb9, 0x6d, 0x56, 0x23,
      0x6b, 0xe1, 0x6c, 0x1a, 0x7e, 0x1a, 0xaa, 0x68, 0xec, 0x15, 0x29, 0xda,
      0xb2, 0xee, 0x3d, 0xe5, 0x43, 0x39, 0x66, 0x2a, 0xf5, 0x3a, 0x38, 0xb8,
      0x5e, 0x3d, 0x8a, 0xd3, 0x2d, 0x01, 0xd9, 0x15, 0xdf, 0x58, 0x1c, 0x84,
      0xc5, 0x32, 0x5a, 0xa0, 0x11, 0xde, 0x98, 0x39, 0x5e, 0x43, 0x9f, 0xd5,
      0x58, 0x38, 0x88, 0xa0, 0xb6, 0xba, 0xc0, 0x23, 0x52, 0x54, 0xb8, 0x14,
      0xea, 0x80, 0xaa, 0x15, 0x62, 0xb7, 0x0a, 0x87, 0x64, 0x38, 0x1d, 0xe7,
      0xd7, 0xea, 0x43, 0x0d, 0x4c, 0x2e, 0x3b, 0xc1, 0x7a, 0xfe, 0xcd, 0x9d,
      0x93, 0xbe, 0xa5, 0x2a, 0x13, 0x2c, 0x6c, 0x97, 0x3d, 0x1a, 0x5c, 0x05,
      0x70, 0xb3, 0x1a, 0x56, 0xc8, 0x7d, 0x1d, 0x2b, 0xfd, 0x83, 0xfa, 0xbf,
      0xc1, 0x08, 0x33, 0xbc, 0x09, 0x89, 0xde, 0x48, 0x33, 0x79, 0x91, 0xb0,
      0x76, 0x2e, 0x85, 0x62, 0x58, 0xe5, 0x91, 0x05, 0xec, 0x61, 0x4e, 0x4a,
      0x88, 0x1c, 0x9e, 0x1e, 0x9f, 0xa2, 0x25, 0x13, 0xc5, 0xa1, 0x17, 0xb7,
      0x9d, 0x80, 0xaf, 0x8b, 0xf5, 0xa1, 0x67, 0x20, 0xe0, 0x47, 0xcc, 0x40,
      0x31, 0x13, 0x11, 0xd6, 0x1c, 0x30, 0x13, 0x1f, 0x2a, 0x56, 0x64, 0x5f,
      0xf3, 0x55, 0x6a, 0x9d, 0x10, 0xf0, 0xeb, 0xdd, 0xb0, 0x4f, 0x54, 0x28,
      0x75, 0xde, 0x18, 0xa5, 0x01, 0x36, 0xa9, 0x9c, 0xee, 0x2a, 0x46, 0x99,
      0x6f, 0x36, 0xc3, 0xd5, 0x09, 0x55, 0xd3, 0xd2, 0x98, 0x06, 0x48, 0x36,
      0x18, 0xa6, 0x3d, 0x1a, 0xdc, 0x97, 0xea, 0x1e, 0x74, 0x88, 0x86, 0xa5,
      0xcc, 0xbf, 0x44, 0x56, 0x28, 0xb1, 0x50, 0x5a, 0xf5, 0x35, 0x5a, 0xf1,
      0x3e, 0x21, 0xb2, 0x12, 0xd1, 0xd7, 0x9b, 0xff, 0x3f, 0x9e, 0xd1, 0xf9,
      0x96, 0x97, 0xde, 0x76, 0x71, 0x79, 0x74, 0x72, 0xa8, 0x72, 0x5c, 0xd1,
      0xf0, 0xc2, 0x0d, 0xa0, 0x2f, 0xac, 0x97, 0xbb, 0xb5, 0x8e, 0x01, 0xfd,
      0xc5, 0x2d, 0x69, 0xcf, 0x30, 0xa1, 0xca, 0x9e, 0x80, 0x4a, 0x71, 0x79,
      0x20, 0x4e, 0x44, 0xc9, 0x61, 0xda, 0x9c, 0x13};
  const std::vector<uint8_t> decrypted = {
      0x01, 0x74, 0x65, 0x73, 0x74, 0x00, 0x02, 0x02, 0xfd, 0x02, 0x29, 0x00,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0x77, 0x77, 0x47, 0x6c, 0x72, 0x5a, 0x48, 0x46, 0x35, 0x67, 0x4b, 0x4e,
      0x36, 0x44, 0x33, 0x6d, 0x44, 0x64, 0x69, 0x68, 0x63, 0x6f, 0x33, 0x6f,
      0x50, 0x65, 0x59, 0x4e, 0x32, 0x4b, 0x46, 0x79, 0x62, 0x4c, 0x39, 0x68,
      0x55, 0x55, 0x46, 0x71, 0x4f, 0x76, 0x6b, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x24, 0x1b, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01,
      0x01, 0x02, 0x00, 0x02, 0x02, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x02,
      0x02, 0x01, 0x00, 0x04, 0x08, 0x00, 0x00, 0x7f, 0x7f, 0x7f, 0x7f, 0x00,
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x02, 0x52, 0x2e, 0x16, 0x62, 0x68,
      0x41, 0x4a, 0x57, 0x47, 0x72, 0x73, 0x61, 0x45, 0x54, 0x67, 0x6c, 0x52,
      0x6a, 0x75, 0x77, 0x78, 0x4d, 0x77, 0x6e, 0x6c, 0x41, 0x2f, 0x64, 0x35,
      0x57, 0x36, 0x45, 0x67, 0x59, 0x57, 0x78, 0x00, 0x01, 0x5d, 0xac, 0x9e,
      0x07, 0xf9, 0xe8, 0xb6, 0xc9, 0x98, 0xc2, 0xb1, 0x98, 0xe9, 0xa1, 0x84,
      0x5e, 0xe8, 0xbf, 0xeb, 0xf3, 0x94, 0x58, 0x9f, 0xb6, 0xcb, 0xea, 0x1c,
      0x07, 0x80, 0x17, 0x26, 0x7c, 0x97, 0x2f, 0xaf, 0x15, 0x05, 0x6d, 0xe0,
      0x22, 0xd5, 0x7c, 0x84, 0x0f, 0x20, 0xf6, 0x67, 0x47, 0x28, 0x3c, 0x44,
      0x64, 0x01, 0x24, 0x0e, 0x8e, 0xd6, 0x09, 0x5f, 0xdc, 0xd5, 0xcf, 0xcc,
      0xe6, 0xe1, 0x0b, 0x02, 0xf0, 0xaf, 0x0b, 0xed, 0x1d, 0xaf, 0x45, 0x28,
      0x38, 0x94, 0xef, 0x7d, 0x0e, 0x6f, 0x4e, 0x7a, 0xaf, 0x68, 0xec, 0x84,
      0xab, 0xf7, 0xd1, 0x97, 0xa1, 0x77, 0x77, 0x59, 0x89, 0x9e, 0xe1, 0xa5,
      0x73, 0xbd, 0x99, 0xb7, 0xcb, 0xa1, 0x6a, 0xbb, 0x8b, 0x20, 0x1f, 0x85,
      0xf3, 0x39, 0x57, 0xa4, 0xc9, 0xdf, 0xba, 0x9c, 0xa4, 0xf8, 0xc9, 0x08,
      0x84, 0x9a, 0x3e, 0xb0, 0x65, 0xee, 0x21, 0xba, 0x03, 0x43, 0xb8, 0x35,
      0x08, 0xab, 0x94, 0x99, 0xc5, 0x24, 0x6f, 0x9f};
  const std::vector<uint8_t> keys = {// 76198126, 73222920, 51067574, 7505250
                                     0x04, 0x8a, 0xb0, 0xee, 0x04, 0x5d,
                                     0x4b, 0x08, 0x03, 0x0b, 0x3a, 0xb6,
                                     0x00, 0x72, 0x85, 0x62};

  std::vector<uint8_t> test_decrypted = {};
  ASSERT_NO_THROW(openrs::common::crypt::DecryptXTEA(
      keys.data(), keys.size(),
      reinterpret_cast<const uint8_t*>(encrypted.data()), encrypted.size(),
      &test_decrypted));
  EXPECT_TRUE(std::equal(decrypted.cbegin(), decrypted.cend(),
                         test_decrypted.cbegin(), test_decrypted.cend()));
}