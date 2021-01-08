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
#include <openrs/game/player.h>

#include <string>

TEST(Player, GenerateRandomString) {
  std::string test_string;
  openrs::game::Player::GenerateRandomString(10, &test_string);
  // GenerateRandomString base64 encodes the random string.
  EXPECT_EQ(16, test_string.size());
}

TEST(Player, EncodePassword) {
  std::string password;
  std::string salt;
  std::string encoded;
  openrs::game::Player::GenerateRandomString(20, &password);
  openrs::game::Player::GenerateRandomString(10, &salt);
  EXPECT_NO_THROW(
      openrs::game::Player::EncodePassword(password, salt, &encoded));
}
