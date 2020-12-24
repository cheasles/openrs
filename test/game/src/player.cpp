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
