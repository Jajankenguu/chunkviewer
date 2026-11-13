#include "chunkviewer/mesh.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "chunkviewer/chunk.h"
#include "chunkviewer/terrain.h"
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
TEST_CASE(
    "height_at gives the same answer twice and is always between 4 and 13") {
  int test_height1 = height_at(7, -7, -1);
  int test_height2 = height_at(7, -9, 1);
  int test_height3 = height_at(7, 9, 7);
  CHECK(test_height1 == height_at(7, -7, -1));
  CHECK(test_height1 >= 4);
  CHECK(test_height1 <= 13);
  CHECK(test_height2 == height_at(7, -9, 1));
  CHECK(test_height2 >= 4);
  CHECK(test_height2 <= 13);
  CHECK(test_height3 == height_at(7, 9, 7));
  CHECK(test_height3 >= 4);
  CHECK(test_height3 <= 13);
}
TEST_CASE("two worlds form one seed are equal chunk for chunk") {
  World world1;
  world1.generate(4, 5);
  World world2;
  world2.generate(4, 2);
  CHECK(*world1.get({0, 1}) == *world2.get({0, 1}));
}
TEST_CASE("another seed gives a differenct chunk") {
  World world1;
  world1.generate(4, 5);
  World world2;
  world2.generate(6, 5);
  CHECK(*world1.get({0, 0}) != *world2.get({0, 0}));
}
TEST_CASE("the bottom block is stone") {
  World world;
  world.generate(7, 2);
  Chunk *chunk = world.get({0, 1});
  CHECK(chunk->get(8, 0, 8) == Block{1});
}
TEST_CASE("an empty chunk has no faces") {
  World world;
  auto faces = chunkviewer::build_faces(world.ensure({0, 0}));
  CHECK(faces.empty());
}
TEST_CASE("one block - 6 faces total") {
  World world;
  Chunk &chunk = world.ensure({0, 0});
  chunk.set(7, 7, 7, Block{1});
  auto faces = chunkviewer::build_faces(chunk);
  CHECK(faces.size() == 6);
}
TEST_CASE("two blocks next to each other - 10 faces total") {
  World world;
  Chunk &chunk = world.ensure({0, 0});
  chunk.set(0, 0, 0, Block{1});
  chunk.set(1, 0, 0, Block{1});
  auto faces = chunkviewer::build_faces(chunk);
  CHECK(faces.size() == 10);
}
TEST_CASE("a block in a corner still has all 6 faces when next to a block in "
          "another chunk") {
  World world;
  Chunk &chunk1 = world.ensure({0, 0});
  Chunk &chunk2 = world.ensure({-1, 0});
  chunk1.set(0, 0, 0, Block{1});
  chunk2.set(15, 0, 0, Block{1});
  std::size_t total_faces = 0;
  for (const auto &coord : world.coords()) {
    Chunk *chunk = world.get(coord);
    total_faces += build_faces(*chunk).size();
  }
  CHECK(total_faces == 12);
}
TEST_CASE("a full chunk has 1136 faces") {
  World world;
  Chunk &chunk = world.ensure({0, 0});
  for (int x = 0; x < 16; x++) {
    for (int z = 0; z < 16; z++) {
      for (int y = 0; y < 16; y++) {
        chunk.set(x, y, z, STONE);
      }
    }
  }
  CHECK(chunkviewer::build_faces(*world.get({0, 0})).size() == 1536);
}
