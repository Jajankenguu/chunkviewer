#include "chunkviewer/mesh.h"
#include "chunkviewer/save.h"
#include "chunkviewer/world.h"
#include "imgui.h"
#include "render.h"
#include "rlImGui.h"
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <filesystem>
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
  std::filesystem::path path;
  bool saving = false;
  bool loading = false;
};

World generate_world(Config config) {
  World world;
  world.generate(config.seed, config.radius);
  return world;
}

void launch_window(Config config, World &world) {
  const int screen_width = 1280;
  const int screen_height = 720;
  InitWindow(screen_width, screen_height, "Chunkviewer 0.1");
  SetTargetFPS(60);
  rlImGuiSetup(true);

  int frame_count = 0;
  int seed_ui = config.seed;
  int radius_ui = config.radius;
  bool wireframe = false;
  bool fullscreen = false;

  std::vector<render::ChunkModel> models = render::build_world(world);
  Camera3D camera{};
  camera.position = {40, 30, 40};
  camera.target = {8, 6, 8};
  camera.up = {0, 1, 0};
  camera.fovy = 60;
  camera.projection = CAMERA_PERSPECTIVE;

  while (!WindowShouldClose() && frame_count != config.frames) {
    UpdateCamera(&camera, CAMERA_ORBITAL);
    if ((IsWindowFullscreen() && !fullscreen) ||
        (!IsWindowFullscreen() && fullscreen)) {
      ToggleFullscreen();
    }
    BeginDrawing();
    ClearBackground(SKYBLUE);
    BeginMode3D(camera);
    render::draw(models, wireframe);
    DrawGrid(64, 1.0f);
    EndMode3D();

    rlImGuiBegin();
    ImGui::Begin("chunkviewer");
    ImGui::Text("%.1f fps", ImGui::GetIO().Framerate);
    ImGui::Text("chunks %zu blocks %lld faces %zu", world.size(),
                world.total_blocks(), render::total_faces(models));
    if (ImGui::Button("regenerate")) {
      config.seed = seed_ui;
      config.radius = radius_ui;
      world = generate_world(config);
      render::unload(models);
      models = render::build_world(world);
    }
    ImGui::SliderInt("seed", &seed_ui, 0, 9999);
    ImGui::SliderInt("radius", &radius_ui, 0, 6);
    ImGui::Checkbox("wireframe", &wireframe);
    ImGui::Checkbox("fullscreen", &fullscreen);
    ImGui::End();
    rlImGuiEnd();

    frame_count++;
    EndDrawing();
  }
  render::unload(models);
  rlImGuiShutdown();
  CloseWindow();
  std::cout << std::format(
      "chunkviewer ran {} frames, {} chunks, {} blocks, {} faces, ui {}\n",
      frame_count, world.size(), world.total_blocks(), count_faces(world),
      IMGUI_VERSION);
}

bool parse_args(int argc, char *argv[], Config &config) {
  const std::vector<std::string_view> args(argv + 1, argv + argc);

  for (size_t i = 0; i < args.size(); i++) {
    const std::string_view arg = args[i];

    if (arg == "--frames") {
      if (i + 1 >= args.size()) {
        std::cout << "usage: --frames N\n";
        return false;
      }
      std::string_view frames = args[++i];
      auto [ptr, ec] = std::from_chars(
          frames.data(), frames.data() + frames.size(), config.frames);
      if (ec != std::errc{}) {
        std::cout << std::format("invalid frames input: {}\n", ptr);
        return false;
      }
      if (config.frames < 1 || config.frames > 9999) {
        std::cout << std::format("frames input out of range (1-9999): {}\n",
                                 ptr);
        return false;
      }
    }
    if (arg == "--stats") {
      config.stats = true;
    }
    if (arg == "--seed") {
      if (i + 1 >= args.size()) {
        std::cout << "usage: --seed N\n";
        return false;
      }
      std::string_view seed = args[++i];
      auto [ptr, ec] =
          std::from_chars(seed.data(), seed.data() + seed.size(), config.seed);
      if (ec != std::errc{}) {
        std::cout << std::format("invalid seed input: {}\n", ptr);
        return false;
      }
    }
    if (arg == "--radius") {
      if (i + 1 >= args.size()) {
        std::cout << "usage: --radius N\n";
        return false;
      }
      std::string_view radius = args[++i];
      auto [ptr, ec] = std::from_chars(
          radius.data(), radius.data() + radius.size(), config.radius);
      if (ec != std::errc{}) {
        std::cout << std::format("invalid radius input: {}\n", ptr);
        return false;
      }
      if (config.radius < 0 || config.radius > 6) {
        std::cout << std::format("radius input out of range (0-6): {}\n", ptr);
        return false;
      }
    }
    if (arg == "--save") {
      if (i + 1 >= args.size()) {
        std::cout << "usage: --save FILE\n";
        return false;
      }
      config.path = args[++i];
      if (!config.path.has_extension() || config.path.extension() != ".world") {
        std::cout << std::format("invalid path input: {}\n",
                                 config.path.string());
        return false;
      }
      config.saving = true;
    }
    if (arg == "--load") {
      if (i + 1 >= args.size()) {
        std::cout << "usage: --load FILE\n";
        return false;
      }
      config.path = args[++i];
      if (!config.path.has_extension() || config.path.extension() != ".world") {
        std::cout << std::format("invalid path input: {}\n",
                                 config.path.string());
        return false;
      }
      config.loading = true;
    }
  }
  return true;
}

int main(int argc, char *argv[]) {
  Config config;
  if (!parse_args(argc, argv, config)) {
    return 1;
  }

  World world = generate_world(config);
  if (config.loading) {
    if (!load_world(world, config.path)) {
      std::cerr << std::format("cannot load {}\n", config.path.string());
      return 1;
    }
  } else if (config.saving) {
    if (!save_world(world, config.path)) {
      std::cerr << std::format("cannot save to {}\n", config.path.string());
      return 1;
    }
    std::cout << std::format("saved {}\n", config.path.string());
    config.stats = true;
  }
  if (config.stats) {
    std::cout << std::format("chunks {} blocks {} faces {}\n", world.size(),
                             world.total_blocks(), count_faces(world));
    return 0;
  }

  launch_window(config, world);

  return 0;
}
