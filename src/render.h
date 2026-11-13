#ifndef CHUNKVIEWER_RENDER_H
#define CHUNKVIEWER_RENDER_H

#include "chunkviewer/chunk.h"
#include "chunkviewer/world.h"
#include "raylib.h"
#include <cstddef>
#include <vector>

namespace chunkviewer::render {

struct ChunkModel {
  ChunkCoord at;
  Model model;
  std::size_t faces;
};

ChunkModel build_model(const Chunk &chunk, ChunkCoord at);
std::vector<ChunkModel> build_world(const World &world);
void unload(std::vector<ChunkModel> &models);
void draw(const std::vector<ChunkModel> &models, bool wireframe);
std::size_t total_faces(const std::vector<ChunkModel> &models);

} // namespace chunkviewer::render

#endif
