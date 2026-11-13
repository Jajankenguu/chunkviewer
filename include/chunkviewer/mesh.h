#ifndef MESH_H
#define MESH_H

#include "chunkviewer/chunk.h"
#include <vector>

namespace chunkviewer {

struct Face {
  int x, y, z;
  int dir;
  Block block;
  bool operator==(const Face &) const = default;
};

std::vector<Face> build_faces(const Chunk &chunk);

} // namespace chunkviewer

#endif
