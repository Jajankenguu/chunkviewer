#include <format>
#include <iostream>
#include <raylib.h>

int main(int argc, char *argv[]) {
  int frames = -1;
  if (argc > 3) {
    std::cout << "usage: --frames N";
    return 1;
  }

  if (strcmp(argv[1], "--frames") == 0 && argv[2]) {
    int frames_input = atoi(argv[2]);
    if (frames_input < 1) {
      std::cout << "frames cannot be zero or negative";
      return 1;
    }
    frames = frames_input;
  } else {
    std::cout << "usage: --frames N";
  }

  const int screen_width = 1280;
  const int screen_height = 720;
  InitWindow(screen_width, screen_height, "Chunkviewer 0.1");
  SetTargetFPS(60);
  int frame_count = 0;

  while (!WindowShouldClose() && frame_count != frames) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText(std::format("Frame count: {}", frame_count).c_str(), 190, 200, 20,
             LIGHTGRAY);
    frame_count++;
    EndDrawing();
  }
  std::cout << std::format("ran {} frames\n", frame_count);
  CloseWindow();
  return 0;
}
