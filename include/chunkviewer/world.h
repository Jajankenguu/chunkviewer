#ifndef WORLD_H
#define WORLD_H

#include <chunkviewer/chunk.h>
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

private:
  std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash>
      chunks_;
};

} // namespace chunkviewer

#endif
