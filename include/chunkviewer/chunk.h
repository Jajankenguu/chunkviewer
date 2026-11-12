#ifndef CHUNK_H
#define CHUNK_H

#include <array>
#include <cstddef>
#include <cstdint>

namespace chunkviewer {

using Block = std::uint8_t;

constexpr Block AIR = 0, STONE = 1, DIRT = 2, GRASS = 3;
struct ChunkCoord {
  int x = 0;
  int z = 0;
  bool operator==(const ChunkCoord &) const = default;
};
struct ChunkCoordHash {
  std::size_t operator()(ChunkCoord chunk_coord) const {
    return std::hash<long long>{}(
        (static_cast<long long>(chunk_coord.x) << 32) ^
        static_cast<unsigned>(chunk_coord.z));
  };
};

class Chunk {
public:
  static constexpr int SIZE = 16;
  static constexpr int VOLUME = 4096;

  Block get(int x, int y, int z) const {
    int index = (y * 16 + z) * 16 + x;
    return blocks_[index];
  };
  void set(int x, int y, int z, Block block) {
    int index = (y * 16 + z) * 16 + x;
    blocks_[index] = block;
  };
  static bool inside(int x, int y, int z) {
    return x >= 0 && x < 16 && y >= 0 && y < 16 && z >= 0 && z < 16;
  };
  int count() const;
  bool operator==(const Chunk &) const = default;

private:
  std::array<Block, VOLUME> blocks_{};
};

} // end namespace chunkviewer

#endif
