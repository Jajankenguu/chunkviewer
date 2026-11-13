#include "chunkviewer/terrain.h"
#include "chunkviewer/chunk.h"

namespace {

int floor_div(int a, int b) { return a >= 0 ? a / b : -((-a + b - 1) / b); }

} // namespace

namespace chunkviewer {

std::uint32_t hash2(std::uint32_t seed, int x, int z) {
  std::uint32_t h = seed ^ (static_cast<std::uint32_t>(x) * 374761393u) ^
                    (static_cast<std::uint32_t>(z) * 668265263u);
  h = (h ^ (h >> 13)) * 1274126177u;
  return h ^ (h >> 16);
}

int height_at(std::uint32_t seed, int wx, int wz) // 4..13
{
  int cx = floor_div(wx, 8),
      cz = floor_div(
          wz, 8); // one value per 8 x 8 cell, averaged with three neighbours
  int sum = 0;
  for (int dx = 0; dx <= 1; ++dx)
    for (int dz = 0; dz <= 1; ++dz)
      sum += static_cast<int>(hash2(seed, cx + dx, cz + dz) % 10u);
  return 4 + sum / 4;
}
void generate_chunk(Chunk &chunk, ChunkCoord at, std::uint32_t seed) {
  for (int x = 0; x < 16; x++) {
    for (int z = 0; z < 16; z++) {
      int height = height_at(seed, at.x * 16 + x, at.z * 16 + z);
      for (int y = 0; y <= height; y++) {
        Block block{0};
        if (y < height - 3) {
          block = {1};
        } else if (y < height - 1) {
          block = {2};
        } else if (y == height - 1) {
          block = {3};
        }
        chunk.set(x, y, z, block);
      }
    }
  }
}

} // namespace chunkviewer
