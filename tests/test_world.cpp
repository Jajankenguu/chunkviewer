#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "chunkviewer/chunk.h"
#include "chunkviewer/world.h"

using namespace chunkviewer;

TEST_CASE("new chunk is empty and remembers a block that is set") {
  Chunk chunk;
  CHECK(chunk.count() == 0);
  Block block{2};
  chunk.set(0, 1, 2, block);
  CHECK(chunk.count() == 1);
  CHECK(chunk.get(0, 1, 2) == block);
}
TEST_CASE("15 is inside, and 16 or -1 is not inside") {
  CHECK(Chunk::inside(15, 5, 1) == true);
  CHECK(Chunk::inside(16, 16, 16) == false);
  CHECK(Chunk::inside(-1, -1, -1) == false);
}
TEST_CASE("get on empty world is nullptr") {
  World world;
  CHECK(world.get(ChunkCoord{1, 1}) == nullptr);
}
TEST_CASE("ensure twice gives same chunk") {
  World world;
  Chunk &chunk = world.ensure(ChunkCoord{0, 0});
  CHECK(&world.ensure(ChunkCoord{0, 0}) == &chunk);
}
TEST_CASE("chunks (1, 0) and (0, 1) are different chunks") {
  World world;
  Chunk &chunk1 = world.ensure(ChunkCoord{1, 0});
  Chunk &chunk2 = world.ensure(ChunkCoord{0, 1});
  CHECK(&chunk1 != &chunk2);
}
TEST_CASE("block across a chunk border") {
  World world;
  ChunkCoord coord{-1, 0};
  world.ensure(coord);
  Block block{3};
  world.get(coord)->set(15, 0, 0, block);
  CHECK(world.block(-1, 0, 0) == block);
}
TEST_CASE("erase once and twice") {
  World world;
  ChunkCoord coord{1, 1};
  world.ensure(coord);
  CHECK(world.erase(coord) == true);
  CHECK(world.erase(coord) == false);
}
