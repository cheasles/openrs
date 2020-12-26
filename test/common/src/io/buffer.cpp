#include <gtest/gtest.h>
#include <openrs/common/io/buffer.h>

#include <algorithm>

TEST(Buffer, ShiftNegative) {
  openrs::common::io::Buffer<> buffer;
  // Test basic uint8_t shifts.
  EXPECT_EQ(128, buffer.ShiftNegative<uint8_t>(0, 128));
  EXPECT_EQ(0, buffer.ShiftNegative<uint8_t>(128, 128));
  EXPECT_EQ(251, buffer.ShiftNegative<uint8_t>(133, 128));
  // Test uint16_t shifts.
  EXPECT_EQ(128, buffer.ShiftNegative<uint16_t>(0, 128));
  EXPECT_EQ(0, buffer.ShiftNegative<uint16_t>(128, 128));
  EXPECT_EQ(65531, buffer.ShiftNegative<uint16_t>(133, 128));
}

TEST(Buffer, ShiftPositive) {
  openrs::common::io::Buffer<> buffer;
  // Test basic uint8_t shifts.
  EXPECT_EQ(128, buffer.ShiftPositive<uint8_t>(0, 128));
  EXPECT_EQ(0, buffer.ShiftPositive<uint8_t>(128, 128));
  EXPECT_EQ(5, buffer.ShiftPositive<uint8_t>(133, 128));
  // Test uint16_t shifts.
  EXPECT_EQ(128, buffer.ShiftPositive<uint16_t>(0, 128));
  EXPECT_EQ(0, buffer.ShiftPositive<uint16_t>(128, 128));
  EXPECT_EQ(5, buffer.ShiftPositive<uint16_t>(133, 128));
}

TEST(Buffer, PutDataVBE) {
  openrs::common::io::Buffer<> buffer;
  std::vector<uint8_t> result = {0xCC, 0xDD, 0xAA, 0xBB};
  ASSERT_NO_THROW(buffer.PutDataVBE<uint32_t>(0xAABBCCDD));
  EXPECT_TRUE(std::equal(result.cbegin(), result.cend(), buffer.cbegin(),
                         buffer.cend()));
}

TEST(Buffer, PutDataVLE) {
  openrs::common::io::Buffer<> buffer;
  std::vector<uint8_t> result = {0xBB, 0xAA, 0xDD, 0xCC};
  ASSERT_NO_THROW(buffer.PutDataVLE<uint32_t>(0xAABBCCDD));
  EXPECT_TRUE(std::equal(result.cbegin(), result.cend(), buffer.cbegin(),
                         buffer.cend()));
}
