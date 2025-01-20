#pragma once
#include "math.h"
#include "Settings.h"

namespace Util {
static inline int getMod(int pos, int mod) { return ((pos % mod) + mod) % mod; }

static inline glm::ivec3 chunkWorldPosToLocalPos(glm::ivec3 world_pos) {
  return {getMod(world_pos.x, Settings::CHUNK_SIZE), 
           getMod(world_pos.y, Settings::CHUNK_SIZE),
          getMod(world_pos.z, Settings::CHUNK_SIZE)};
}

// May overflow when near INT_MAX
// b parameter has to be positive
static inline int roundDownDivide(int a, int b) {
  if (a >= 0)
    return a / b;
  else
    return (a - b + 1) / b;
}

static inline glm::ivec3 worldPosToChunkPos(glm::vec3 world_pos) {
  int x = std::floor((world_pos.x / (float)Settings::CHUNK_SIZE));
  int y = std::floor((world_pos.y / (float)Settings::CHUNK_SIZE));
  int z = std::floor((world_pos.z / (float)Settings::CHUNK_SIZE));
  return {x, y, z};
}
}  // namespace Util
