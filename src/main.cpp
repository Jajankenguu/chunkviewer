#include "chunkviewer/mesh.h"
#include "chunkviewer/world.h"
#include "render.h"
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iostream>
#include <raylib.h>
#include <string_view>
#include <vector>

namespace {
std::size_t count_faces(const chunkviewer::World &world) {
  std::size_t n = 0;
  for (chunkviewer::ChunkCoord c : world.coords())
    n += chunkviewer::build_faces(*world.get(c)).size();
  return n;
}
} // namespace

using namespace chunkviewer;

struct Config {
  int frames = -1;
  bool stats = false;
  std::uint32_t seed = 7;
  int radius = 2;
};

void launch_window(Config config, const World &world) {
  const int screen_width = 1280;
  const int screen_height = 720;
  InitWindow(screen_width, screen_height, "Chunkviewer 0.1");
  SetTargetFPS(60);
  int frame_count = 0;

  std::vector<render::ChunkModel> models = render::build_world(world);
  Camera3D camera{};
  camera.position = {40, 30, 40};
  camera.target = {8, 6, 8};
  camera.up = {0, 1, 0};
  camera.fovy = 60;
  camera.projection = CAMERA_PERSPECTIVE;

  while (!WindowShouldClose() && frame_count != config.frames) {
    UpdateCamera(&camera, CAMERA_ORBITAL);
    BeginDrawing();
    ClearBackground(SKYBLUE);
    BeginMode3D(camera);
    render::draw(models, false);
    DrawGrid(64, 1.0f);
    EndMode3D();
    DrawText(std::format("Frame count: {}", frame_count).c_str(), 100, 100, 20,
             BLACK);
    frame_count++;
    EndDrawing();
  }
  render::unload(models);
  std::cout << std::format("chunkviewer ran {} frames, {} chunks, {} faces",
                           frame_count, world.size(), count_faces(world));
  CloseWindow();
}

bool parse_args(int argc, char *argv[], Config &config) {
  const std::vector<std::string_view> args(argv + 1, argv + argc);

  for (size_t i = 0; i < args.size(); i++) {
    const std::string_view arg = args[i];

    if (arg == "--frames") {
      if (i + 1 >= args.size()) {
        std::cout << "usage: --frames N";
        return false;
      }
      std::string_view frames = args[++i];
      auto [ptr, ec] = std::from_chars(
          frames.data(), frames.data() + frames.size(), config.frames);
      if (ec != std::errc{} || config.frames < 1) {
        std::cout << std::format("invalid frames input: {}", ptr);
        return false;
      }
    }
    if (arg == "--stats") {
      config.stats = true;
    }
    if (arg == "--seed") {
      if (i + 1 >= args.size()) {
        std::cout << "usage: --seed N";
        return false;
      }
      std::string_view seed = args[++i];
      auto [ptr, ec] =
          std::from_chars(seed.data(), seed.data() + seed.size(), config.seed);
      if (ec != std::errc{}) {
        std::cout << std::format("invalid seed input: {}", ptr);
        return false;
      }
    }
    if (arg == "--radius") {
      if (i + 1 >= args.size()) {
        std::cout << "usage: --radius N";
        return false;
      }
      std::string_view radius = args[++i];
      auto [ptr, ec] = std::from_chars(
          radius.data(), radius.data() + radius.size(), config.radius);
      if (ec != std::errc{}) {
        std::cout << std::format("invalid radius input: {}", ptr);
        return false;
      }
    }
  }
  return true;
}

int main(int argc, char *argv[]) {
  Config config;
  if (!parse_args(argc, argv, config)) {
    return 1;
  }
  chunkviewer::World world;
  world.generate(config.seed, config.radius);
  if (config.stats) {
    std::size_t total_faces = 0;
    for (const auto &coord : world.coords()) {
      Chunk *chunk = world.get(coord);
      total_faces += build_faces(*chunk).size();
    }
    std::cout << std::format("chunks {} blocks {} faces {}\n", world.size(),
                             world.total_blocks(), total_faces);
    return 0;
  }
  launch_window(config, world);

  return 0;
}
