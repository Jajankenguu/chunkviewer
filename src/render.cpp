#include "render.h"
#include "chunkviewer/chunk.h"
#include "chunkviewer/mesh.h"
#include "raylib.h"

namespace {

struct FaceShape {
  float corners[4][3];
  float normal[3];
};
constexpr FaceShape SHAPES[6] = {
    {{{1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}}, {1, 0, 0}},  // +x
    {{{0, 0, 1}, {0, 1, 1}, {0, 1, 0}, {0, 0, 0}}, {-1, 0, 0}}, // -x
    {{{0, 1, 0}, {0, 1, 1}, {1, 1, 1}, {1, 1, 0}}, {0, 1, 0}},  // +y, the top
    {{{0, 0, 1}, {0, 0, 0}, {1, 0, 0}, {1, 0, 1}},
     {0, -1, 0}},                                              // -y, the bottom
    {{{1, 0, 1}, {1, 1, 1}, {0, 1, 1}, {0, 0, 1}}, {0, 0, 1}}, // +z
    {{{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}}, {0, 0, -1}}, // -z
};
} // namespace

namespace chunkviewer::render {

struct Colours {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a = 255;
};

constexpr Colours STONE_C = {128, 128, 128}, DIRT_C = {134, 96, 67},
                  GRASS_C = {96, 168, 64};

ChunkModel build_model(const Chunk &chunk, ChunkCoord at) {
  auto faces = chunkviewer::build_faces(chunk);
  Mesh mesh{};
  mesh.vertexCount = static_cast<int>(4 * faces.size());
  mesh.triangleCount = static_cast<int>(2 * faces.size());
  mesh.vertices =
      static_cast<float *>(MemAlloc(mesh.vertexCount * 3 * sizeof(float)));
  mesh.normals =
      static_cast<float *>(MemAlloc(mesh.vertexCount * 3 * sizeof(float)));
  mesh.colors = static_cast<unsigned char *>(
      MemAlloc(mesh.vertexCount * 4 * sizeof(unsigned char)));
  mesh.indices = static_cast<unsigned short *>(
      MemAlloc(mesh.triangleCount * 3 * sizeof(unsigned short)));

  int v = 0;
  int i = 0;
  for (const auto &face : faces) {
    FaceShape shape = SHAPES[face.dir];
    unsigned short base = static_cast<unsigned short>(v);
    for (auto &corner : shape.corners) {
      mesh.vertices[v * 3 + 0] = corner[0] + face.x;
      mesh.vertices[v * 3 + 1] = corner[1] + face.y;
      mesh.vertices[v * 3 + 2] = corner[2] + face.z;
      mesh.normals[v * 3 + 0] = shape.normal[0];
      mesh.normals[v * 3 + 1] = shape.normal[1];
      mesh.normals[v * 3 + 2] = shape.normal[2];
      Colours colour;
      switch (face.block) {
      case (STONE): {
        colour = STONE_C;
        break;
      }
      case (DIRT): {
        colour = DIRT_C;
        break;
      }
      case (GRASS): {
        colour = GRASS_C;
        break;
      }
      }
      mesh.colors[v * 4 + 0] = colour.r;
      mesh.colors[v * 4 + 1] = colour.g;
      mesh.colors[v * 4 + 2] = colour.b;
      mesh.colors[v * 4 + 3] = colour.a;
      v++;
    }
    const unsigned short tri[6] = {base,
                                   static_cast<unsigned short>(base + 1),
                                   static_cast<unsigned short>(base + 2),
                                   base,
                                   static_cast<unsigned short>(base + 2),
                                   static_cast<unsigned short>(base + 3)};
    for (unsigned short t : tri) {
      mesh.indices[i++] = t;
    }
  }
  UploadMesh(&mesh, false);

  return {at, LoadModelFromMesh(mesh), faces.size()};
}

std::vector<ChunkModel> build_world(const World &world) {
  std::vector<ChunkModel> models;
  for (const auto &coord : world.coords()) {
    models.push_back(build_model(*world.get(coord), coord));
  }
  return models;
}

void unload(std::vector<ChunkModel> &models) {
  for (auto &model : models) {
    UnloadModel(model.model);
  }
  models.clear();
}

void draw(const std::vector<ChunkModel> &models, bool wireframe) {
  for (const auto &model : models) {
    Vector3 position{static_cast<float>(model.at.x * 16), 0,
                     static_cast<float>(model.at.z * 16)};
    if (wireframe) {
      DrawModelWires(model.model, position, 1.0f, WHITE);
    } else {
      DrawModel(model.model, position, 1.0f, WHITE);
    }
  }
}

std::size_t total_faces(const std::vector<ChunkModel> &models) {
  std::size_t total = 0;
  for (const auto &model : models) {
    total += model.faces;
  }
  return total;
}

} // namespace chunkviewer::render
