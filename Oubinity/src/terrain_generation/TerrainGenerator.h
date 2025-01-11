#pragma once
#include "../Settings.h"
#include "../chunk/Chunk.h"
#include "../chunk/ChunkSize.h"
#include "preloaded_generation/PreloadedGenerator.h"
#include "procedural_generation/ProceduralGenerator.h";

class TerrainGenerator {
 public:
  TerrainGenerator() = delete;
  TerrainGenerator(int world_seed, uint8_t water_height);
  ~TerrainGenerator() = default;
  // true if chunk is visible, false if it's not
  bool generateChunkTerrain(Chunk& chunk);
  void generateChunkTerrain(Chunk& chunk, ProceduralHeightMap& height_map,
                            bool is_chunk_visible);
  void generateTrees(Chunk& chunk);
  bool isChunkBelowOrAboveSurface(Chunk& chunk, const HeightMap& height_map);
  bool isChunkBelowOrAboveSurface(glm::ivec3 chunk_pos,
                                  const HeightMap& height_map,
                                  LevelOfDetail::LevelOfDetail lod);
  ProceduralHeightMap generateProceduralHeightMap(Chunk& chunk);
  ProceduralHeightMap generateProceduralHeightMap(
      glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod);
  bool generateProceduralLayers(Chunk& chunk, ProceduralHeightMap height_map);
  uint8_t getWaterHeight();
#if defined(SETTING_USE_PRELOADED_HEIGHTMAP) == 1 || \
    defined(SETTING_USE_PRELOADED_COLORMAP) == 1
  bool generatePreloadedUndergroundLayer(Chunk& chunk, HeightMap& height_map);
  PreloadedHeightMap generatePreloadedHeightMap(
      glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod);
#endif

#if SETTING_USE_PRELOADED_COLORMAP
  bool generatePreloadedLayers(Chunk& chunk, HeightMap height_map);
#endif

#if defined(SETTING_USE_HEIGHTMAP_BLENDING) == 1
  PreloadedHeightMap generateBlendedHeightMap(glm::ivec3 chunk_pos,
                                              LevelOfDetail::LevelOfDetail lod);
#endif

 private:
  ProceduralGenerator m_procedural_generator;
#if defined(SETTING_USE_PRELOADED_HEIGHTMAP) == 1 || \
    defined(SETTING_USE_PRELOADED_COLORMAP) == 1
  PreloadedGenerator m_preloaded_generator;
#endif
  uint8_t m_water_height;
};
