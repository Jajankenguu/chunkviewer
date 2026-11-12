#include <chunkviewer/chunk.h>

namespace chunkviewer {

int Chunk::count() const {
  int total = 0;
  for (int i = 0; i < Chunk::VOLUME; i++) {
    if (blocks_[i] != AIR) {
      total++;
    }
  }
  return total;
}

} // namespace chunkviewer
