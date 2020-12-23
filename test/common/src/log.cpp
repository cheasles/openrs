#include <openrs/common/log.h>

#include "gtest/gtest.h"

TEST(Log, GetLabel) {
  EXPECT_EQ("DEBUG", openrs::common::Log::GetLabel(
                         openrs::common::Log::LogLevel::kDebug));
}
