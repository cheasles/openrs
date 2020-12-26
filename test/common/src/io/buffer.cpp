#include <openrs/common/io/buffer.h>

#include "gtest/gtest.h"

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
