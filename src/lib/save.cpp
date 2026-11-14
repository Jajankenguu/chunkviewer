#include "chunkviewer/save.h"
#include "chunkviewer/chunk.h"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string_view>
#include <system_error>

namespace {

template <typename T> bool put(std::ostream &out, const T &value) {
  out.write(reinterpret_cast<const char *>(&value), sizeof value);
  return static_cast<bool>(out);
}
template <typename T> bool take(std::istream &in, T &value) {
  in.read(reinterpret_cast<char *>(&value), sizeof value);
  if (!in) {
    return false;
  }
  return true;
}

} // namespace

namespace chunkviewer {

constexpr char MAGIC[8] = {'C', 'H', 'U', 'N', 'K', 'V', 'W', '1'};

bool save_world(const World &world, const std::filesystem::path &path) {
  if (path.has_parent_path()) {
    std::error_code ec;
    if (std::filesystem::create_directories(path.parent_path(), ec)) {
      std::cerr << "created directories for specifified path\n";
    }
    if (ec) {
      std::cerr << "failed to create directories: " << ec.message() << '\n';
    }
  }
  std::ofstream out(path, std::ios::binary);
  if (!out) {
    std::cerr << "failed to open " << path << " for writing";
  }
  if (!put(out, MAGIC) || !put(out, static_cast<std::uint32_t>(world.seed())) ||
      !put(out, static_cast<std::int32_t>(world.radius())) ||
      !put(out, static_cast<std::uint32_t>(world.size()))) {
    std::cerr << "failed to write magic or world config to the file\n";
    return false;
  }
  for (const auto &coord : world.coords()) {
    if (!put(out, static_cast<std::int32_t>(coord.x)) ||
        !put(out, static_cast<std::int32_t>(coord.z)) ||
        !put(out, world.get(coord)->raw())) {
      std::cerr << "failed to write chunk to file\n";
      return false;
    }
  }
  return true;
}

bool load_world(World &world, const std::filesystem::path &path) {
  World loaded;
  std::ifstream in(path, std::ios::binary);

  char magic[8];
  if (!take(in, magic) ||
      std::string_view(magic, 8) != std::string_view(MAGIC, 8)) {
    std::cerr << "filepath is not a chunkviewer version 1 file\n";
    return false;
  }

  std::uint32_t tmp_seed;
  std::int32_t tmp_radius;
  std::uint32_t chunks_count;
  if (!take(in, tmp_seed) || !take(in, tmp_radius) || !take(in, chunks_count)) {
    std::cerr << "failed to load world seed, radius and chunk count\n";
    return false;
  }
  loaded.set_meta(tmp_seed, tmp_radius);

  for (std::uint32_t i = 0; i < chunks_count; i++) {
    std::int32_t x, z;
    if (!take(in, x) || !take(in, z)) {
      std::cerr << "failed to load chunk coords\n";
      return false;
    }
    if (!take(in, loaded.ensure(ChunkCoord{x, z}).raw())) {
      std::cerr << "failed to load chunk block data\n";
      return false;
    }
  }

  world = std::move(loaded);
  return true;
}

} // namespace chunkviewer
