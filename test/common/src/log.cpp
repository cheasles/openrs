#include <openrs/common/log.h>

#include "gtest/gtest.h"

TEST(Log, GetLabel) {
  // This test is named "Negative", and belongs to the "FactorialTest"
  // test case.
  EXPECT_EQ("DEBUG", openrs::common::Log::GetLabel(
                         openrs::common::Log::LogLevel::kDebug));
}
