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
#include <gtest/gtest.h>
#include <openrs/common/io/bitbuffer.h>

#include <algorithm>

TEST(BitBuffer, PutData) {
  openrs::common::io::BitBuffer<> buffer;
  EXPECT_NO_THROW(buffer.PutData<uint8_t>(1, 1));
  EXPECT_EQ(0b10000000, buffer.at(0));
  EXPECT_NO_THROW(buffer.PutData<uint8_t>(1, 0));
  EXPECT_EQ(0b10000000, buffer.at(0));

  EXPECT_NO_THROW(buffer.PutData<uint8_t>(1, 1));
  EXPECT_EQ(0b10100000, buffer.at(0));
  EXPECT_NO_THROW(buffer.PutData<uint8_t>(1, 0));
  EXPECT_EQ(0b10100000, buffer.at(0));

  EXPECT_NO_THROW(buffer.PutData<uint8_t>(1, 1));
  EXPECT_EQ(0b10101000, buffer.at(0));
  EXPECT_NO_THROW(buffer.PutData<uint8_t>(1, 0));
  EXPECT_EQ(0b10101000, buffer.at(0));

  EXPECT_NO_THROW(buffer.PutData<uint8_t>(1, 1));
  EXPECT_EQ(0b10101010, buffer.at(0));
  EXPECT_NO_THROW(buffer.PutData<uint8_t>(1, 0));
  EXPECT_EQ(0b10101010, buffer.at(0));
}
