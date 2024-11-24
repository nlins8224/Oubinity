#pragma once
#include <FastNoise/FastNoise.h>
#include "../../chunk/Chunk.h"

#include <limits>
#include "DecorationGenerator.h"

namespace std {
template <>
struct hash<glm::ivec2> {
  const size_t operator()(const glm::ivec2& vec) const {
    size_t res = 17;
    res = res * 31 + std::hash<int>()(vec.x);
    res = res * 31 + std::hash<int>()(vec.y);
    return res;
  };
};
}  // namespace std

using ChunkHeightMaps = std::unordered_map<glm::ivec2, HeightMap>;

namespace NoiseSettings {
struct Settings {
  float frequency;
  int octaves;
  float lacunarity;
  float fractal_gain;
  float weighted_strength;
};

static const Settings TestSettings{
    .frequency{0.0005},
    .octaves{5},
    .lacunarity{2.5},
    .fractal_gain{0.5},
    .weighted_strength{0.3},
};

static const Settings TreeSettings{
    .frequency{0.02},
    .octaves{3},
    .lacunarity{2.0},
    .fractal_gain{0.5},
    .weighted_strength{0.0},
};

}  // namespace NoiseSettings

class ProceduralGenerator {
 public:
  ProceduralGenerator();
  ProceduralGenerator(int world_seed, uint8_t water_height);
  virtual ~ProceduralGenerator() = default;
  ProceduralHeightMap generateHeightMap(Chunk& chunk);
  ProceduralHeightMap generateHeightMap(glm::ivec3 chunk_pos,
                                        LevelOfDetail::LevelOfDetail lod);
  ProceduralHeightMap generateHeightMap(glm::ivec3 chunk_pos,
                                        LevelOfDetail::LevelOfDetail lod,
                                        NoiseSettings::Settings settings,
                                        int seed);
  bool generateLayers(Chunk& chunk, ProceduralHeightMap height_map);
  void generateTrees(Chunk& chunk);
  void generateTrees(Chunk& chunk, ProceduralHeightMap& height_map);
  uint8_t getWaterHeight();

 private:
  TreePresenceMap generateTreePresenceMap(glm::ivec3 chunk_pos,
                                          LevelOfDetail::LevelOfDetail lod,
                                          NoiseSettings::Settings settings,
                                          int seed);

  int m_world_seed;
  uint8_t m_water_height;
  DecorationGenerator m_decoration_generator;
  std::unordered_map<glm::ivec3, ProceduralHeightMap> m_heightmaps;
};
