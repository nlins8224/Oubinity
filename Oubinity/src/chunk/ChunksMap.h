#pragma once
#include <glm/glm.hpp>
#include <unordered_map>

class Chunk;

namespace std {
template <>
struct hash<glm::ivec3> {
  const size_t operator()(const glm::ivec3& vec) const {
    size_t res = 17;
    res = res * 31 + std::hash<int>()(vec.x);
    res = res * 31 + std::hash<int>()(vec.y);
    res = res * 31 + std::hash<int>()(vec.z);
    return res;
  };
};
}  // namespace std

using ChunksMap = std::unordered_map<glm::ivec3, Chunk>;
