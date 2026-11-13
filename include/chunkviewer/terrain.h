#ifndef TERRAIN_H
#define TERRAIN_H

#include "chunkviewer/chunk.h"
#include <cstdint>
namespace chunkviewer {

std::uint32_t hash2(std::uint32_t seed, int x, int z);
int height_at(std::uint32_t seed, int wx, int wz);
void generate_chunk(Chunk &chunk, ChunkCoord at, std::uint32_t seed);

} // namespace chunkviewer

#endif // !TERRAIN_H
