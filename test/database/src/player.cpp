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
#include <openrs/database/models/player.h>

#include <memory>

/**
 * Test fixture that opens the database connection before each test.
 */
class DatabaseTest : public ::testing::Test {
 protected:
  qtl::sqlite::database database_;

  /**
   * Opens the database object.
   */
  void SetUp() override { database_.open("test-database.db"); }

  /**
   * Closes and cleans up the database file.
   */
  void TearDown() override {
    this->database_.close();
    std::remove("test-database.db");
  }
};

/**
 * Test that the player model table can be created without throwing an
 * exception.
 */
TEST_F(DatabaseTest, PlayerDatabaseCreate) {
  EXPECT_NO_THROW(openrs::database::models::PlayerModel::CreateModelTable<
                  qtl::sqlite::database>(database_));
}

/**
 * Test that the player model can be inserted into the database.
 */
TEST_F(DatabaseTest, PlayerDatabaseInsert) {
  ASSERT_NO_THROW(openrs::database::models::PlayerModel::CreateModelTable<
                  qtl::sqlite::database>(database_));
  openrs::database::models::PlayerModel player_model;
  player_model.username = "USERNAME";

  const auto kResult =
      player_model.template Save<qtl::sqlite::database, qtl::sqlite::statement>(
          database_);
  EXPECT_TRUE(kResult);
}

/**
 * Test that the player model can be selected from the database.
 */
TEST_F(DatabaseTest, PlayerDatabaseSelect) {
  ASSERT_NO_THROW(openrs::database::models::PlayerModel::CreateModelTable<
                  qtl::sqlite::database>(database_));
  openrs::database::models::PlayerModel player_model;
  player_model.username = "USERNAME";

  auto result =
      player_model.template Save<qtl::sqlite::database, qtl::sqlite::statement>(
          database_);
  ASSERT_TRUE(result);

  std::vector<openrs::database::models::PlayerModel> models;
  result = openrs::database::models::PlayerModel::Select(database_, &models);
  EXPECT_TRUE(result);
  EXPECT_EQ(1, models.size());
  EXPECT_STREQ("USERNAME", models[0].username.c_str());
}

/**
 * Test that the player model can be selected from the database by name.
 */
TEST_F(DatabaseTest, PlayerDatabaseSelectWhere) {
  ASSERT_NO_THROW(openrs::database::models::PlayerModel::CreateModelTable<
                  qtl::sqlite::database>(database_));
  openrs::database::models::PlayerModel player_model;
  player_model.username = "USERNAME";

  auto result =
      player_model.template Save<qtl::sqlite::database, qtl::sqlite::statement>(
          database_);
  ASSERT_TRUE(result);

  std::vector<openrs::database::models::PlayerModel> models;
  result = openrs::database::models::PlayerModel::Select(
      database_, "username", player_model.username, &models);
  EXPECT_TRUE(result);
  EXPECT_EQ(1, models.size());
  EXPECT_STREQ("USERNAME", models[0].username.c_str());

  models.clear();
  player_model.username = "NOTAUSER";
  result = openrs::database::models::PlayerModel::Select(
      database_, "username", player_model.username, &models);
  EXPECT_FALSE(result);
  EXPECT_EQ(0, models.size());
}
