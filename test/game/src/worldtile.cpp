#include <openrs/game/worldtile.h>

#include "gtest/gtest.h"

TEST(WorldTile, Accessors) {
  openrs::game::WorldTile world_tile;
  EXPECT_EQ(0, world_tile.x());
  EXPECT_EQ(0, world_tile.y());
  EXPECT_EQ(0, world_tile.z());
  EXPECT_EQ(world_tile.z(), world_tile.plane());
}
