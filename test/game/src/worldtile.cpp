#include <gtest/gtest.h>
#include <openrs/game/worldtile.h>

#include <algorithm>

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

TEST(WorldTile, GetMapRegions) {
  const std::vector<uint32_t> kExpectedMapRegions = {
    11062, 11063, 11064, 11318, 11319, 11320, 11574, 11575, 11576
  };
  openrs::game::WorldTile world_tile(2842, 3550, 1);
  std::vector<uint32_t> map_regions;
  ASSERT_NO_THROW(world_tile.GetMapRegions(6, &map_regions));
  EXPECT_TRUE(std::equal(kExpectedMapRegions.cbegin(),
                         kExpectedMapRegions.cend(), map_regions.cbegin(),
                         map_regions.cend()));
}

TEST(WorldTile, Chunk) {
  openrs::game::WorldTile world_tile(2842, 3550, 1);
  EXPECT_EQ(355, world_tile.chunk_x());
  EXPECT_EQ(443, world_tile.chunk_y());
}
