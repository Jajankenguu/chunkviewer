#include "chunkviewer/mesh.h"
#include "chunkviewer/chunk.h"

namespace chunkviewer {

std::vector<Face> build_faces(const Chunk &chunk) {
  static constexpr int OFFSETS[6][3] = {{1, 0, 0},  {-1, 0, 0}, {0, 1, 0},
                                        {0, -1, 0}, {0, 0, 1},  {0, 0, -1}};
  std::vector<Face> faces;
  for (int y = 0; y < chunk.SIZE; y++) {
    for (int z = 0; z < chunk.SIZE; z++) {
      for (int x = 0; x < chunk.SIZE; x++) {
        Block block = chunk.get(x, y, z);
        if (block != AIR) {
          for (int dir = 0; dir < 6; dir++) {
            int nx = x + OFFSETS[dir][0];
            int ny = y + OFFSETS[dir][1];
            int nz = z + OFFSETS[dir][2];
            if (!Chunk::inside(nx, ny, nz) || chunk.get(nx, ny, nz) == AIR) {
              Face face = {x, y, z, dir, block};
              faces.push_back(face);
            }
          }
        }
      }
    }
  }
  return faces;
}

} // namespace chunkviewer
