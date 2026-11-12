#include "chunkviewer/chunk.h"
#include <algorithm>
#include <chunkviewer/world.h>
#include <optional>

namespace {
int floor_div(int a, int b) { return a >= 0 ? a / b : -((-a + b - 1) / b); }
} // namespace

namespace chunkviewer {

Chunk *World::get(ChunkCoord chunk_coord) {
  auto it = chunks_.find(chunk_coord);
  return it == chunks_.end() ? nullptr : it->second.get();
}

const Chunk *World::get(ChunkCoord chunk_coord) const {
  auto it = chunks_.find(chunk_coord);
  return it == chunks_.end() ? nullptr : it->second.get();
}

Chunk &World::ensure(ChunkCoord chunk_coord) {
  auto &chunk = chunks_[chunk_coord];
  if (!chunk) {
    chunk = std::make_unique<Chunk>();
  }
  return *chunk;
}

bool World::erase(ChunkCoord chunk_coord) {
  return chunks_.erase(chunk_coord) == 1;
}

long long World::total_blocks() const {
  long long total = 0;
  for (const auto &chunk : chunks_) {
    total += chunk.second->count();
  }
  return total;
}

std::vector<ChunkCoord> World::coords() const {
  std::vector<ChunkCoord> coords;
  for (const auto &chunk : chunks_) {
    coords.push_back(chunk.first);
  }
  std::sort(coords.begin(), coords.end(),
            [](const ChunkCoord &a, const ChunkCoord &b) {
              if (a.x == b.x) {
                return a.z < b.z;
              }
              return a.x < b.x;
            });
  return coords;
}

std::optional<Block> World::block(int wx, int wy, int wz) const {
  int chunk_x = floor_div(wx, Chunk::SIZE);
  int chunk_z = floor_div(wz, Chunk::SIZE);
  ChunkCoord chunk_coord{chunk_x, chunk_z};
  const Chunk *chunk = World::get(chunk_coord);
  if (chunk == nullptr) {
    return std::nullopt;
  }
  int block_x = wx - chunk_x * Chunk::SIZE;
  int block_z = wz - chunk_z * Chunk::SIZE;
  if (Chunk::inside(block_x, wy, block_z) == false) {
    return std::nullopt;
  }
  return chunk->get(block_x, wy, block_z);
}

} // namespace chunkviewer
