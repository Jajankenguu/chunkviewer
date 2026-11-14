#ifndef WORLD_H
#define WORLD_H

#include <chunkviewer/chunk.h>
#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace chunkviewer {

class World {
public:
  Chunk *get(ChunkCoord chunk_coord);
  const Chunk *get(ChunkCoord chunk_coord) const;
  Chunk &ensure(ChunkCoord chunk_coord);
  bool erase(ChunkCoord chunk_coord);
  std::size_t size() const { return chunks_.size(); };
  long long total_blocks() const;
  std::vector<ChunkCoord> coords() const;
  std::optional<Block> block(int wx, int wy, int wz) const;
  void generate(std::uint32_t seed, int radius);
  void set_meta(std::uint32_t seed, int radius) {
    seed_ = seed;
    radius_ = radius;
  }
  std::uint32_t seed() const { return seed_; };
  int radius() const { return radius_; };

private:
  std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash>
      chunks_;
  std::uint32_t seed_ = 0;
  int radius_ = 0;
};

} // namespace chunkviewer

#endif
