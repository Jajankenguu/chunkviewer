#ifndef CHUNKVIEWER_SAVE_H
#define CHUNKVIEWER_SAVE_H

#include "chunkviewer/world.h"
#include <filesystem>

namespace chunkviewer {

bool save_world(const World &world, const std::filesystem::path &path);
bool load_world(World &world, const std::filesystem::path &path);

} // namespace chunkviewer

#endif
