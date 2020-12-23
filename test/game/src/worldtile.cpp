#include <openrs/game/worldtile.h>

#include "gtest/gtest.h"

TEST(WorldTile, Accessors) {
  openrs::game::WorldTile world_tile;
  EXPECT_EQ(0, world_tile.x());
  EXPECT_EQ(0, world_tile.y());
  EXPECT_EQ(0, world_tile.z());
  EXPECT_EQ(world_tile.z(), world_tile.plane());
}

TEST(WorldTile, Mutators) {
  openrs::game::WorldTile world_tile;
  world_tile.set_x(5);
  world_tile.set_y(6);
  world_tile.set_z(7);
  EXPECT_EQ(5, world_tile.x());
  EXPECT_EQ(6, world_tile.y());
  EXPECT_EQ(7, world_tile.z());

  world_tile.set_plane(8);
  EXPECT_EQ(world_tile.z(), world_tile.plane());
}
