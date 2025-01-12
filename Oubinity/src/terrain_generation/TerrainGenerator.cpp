#include "TerrainGenerator.h"

#if defined(SETTING_USE_PRELOADED_HEIGHTMAP) == 1 || \
    defined(SETTING_USE_PRELOADED_COLORMAP) == 1
TerrainGenerator::TerrainGenerator(int world_seed, uint8_t water_height)
    : m_water_height{water_height},
      m_procedural_generator{ProceduralGenerator(world_seed, water_height)},
      m_preloaded_generator{PreloadedGenerator(water_height)} {}
#else
TerrainGenerator::TerrainGenerator(int world_seed, uint8_t water_height)
    : m_water_height{water_height},
      m_procedural_generator{ProceduralGenerator(world_seed, water_height)} {}
#endif

bool TerrainGenerator::generateChunkTerrain(Chunk& chunk) {
  ProceduralHeightMap height_map = generateProceduralHeightMap(chunk);
  bool is_chunk_visible = !isChunkBelowOrAboveSurface(chunk, height_map);
  generateChunkTerrain(chunk, height_map, is_chunk_visible);
  return is_chunk_visible;
}

void TerrainGenerator::generateChunkTerrain(Chunk& chunk,
                                            ProceduralHeightMap& height_map,
                                            bool is_chunk_visible) {
  if (is_chunk_visible) {
    chunk.setBlockArray();
    generateProceduralLayers(chunk, height_map);
  }
  chunk.setIsVisible(is_chunk_visible);
}

#if SETTING_USE_PRELOADED_HEIGHTMAP || SETTING_USE_PRELOADED_COLORMAP

PreloadedHeightMap TerrainGenerator::generatePreloadedHeightMap(
    glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod) {
  return m_preloaded_generator.getHeightMap(chunk_pos, lod);
}
#endif

#if SETTING_USE_HEIGHTMAP_BLENDING
PreloadedHeightMap TerrainGenerator::generateBlendedHeightMap(
    glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod) {
  auto preloaded_height_map = std::make_unique<PreloadedHeightMap>(
      m_preloaded_generator.getHeightMap(chunk_pos, lod));
  auto procedural_height_map = std::make_unique<ProceduralHeightMap>(
      m_procedural_generator.generateHeightMap(chunk_pos, lod));
  for (int x = 0; x < lod.block_amount + 2; x++) {
    for (int z = 0; z < lod.block_amount + 2; z++) {
      double proc_height = procedural_height_map->data()[x][z];
      double from_texture_height = preloaded_height_map->data()[x][z];
      preloaded_height_map->data()[x][z] =
          from_texture_height * 0.8f + proc_height * 0.2f;
    }
  }
  return *preloaded_height_map;
}
#endif

#if SETTING_USE_PRELOADED_COLORMAP
bool TerrainGenerator::generatePreloadedLayers(Chunk& chunk,
                                               HeightMap height_map) {
  chunk.setBlockArray();
  BlockMap block_map = m_preloaded_generator.getBlockMap(
      chunk.getPos(), chunk.getLevelOfDetail());
  return m_preloaded_generator.generateLayers(chunk, height_map, block_map);
}
#endif

uint8_t TerrainGenerator::getWaterHeight() { return m_water_height; }

ProceduralHeightMap TerrainGenerator::generateProceduralHeightMap(
    Chunk& chunk) {
  return m_procedural_generator.generateHeightMap(chunk.getPos(),
                                                  chunk.getLevelOfDetail());
}

ProceduralHeightMap TerrainGenerator::generateProceduralHeightMap(
    glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod) {
  return m_procedural_generator.generateHeightMap(chunk_pos, lod);
}

bool TerrainGenerator::generateProceduralLayers(
    Chunk& chunk, ProceduralHeightMap height_map) {
  return m_procedural_generator.generateLayers(chunk, height_map);
}

void TerrainGenerator::generateTrees(Chunk& chunk) {
#if SETTING_USE_PRELOADED_HEIGHTMAP
#if SETTING_USE_PRELOADED_TREEMAP
  m_preloaded_generator.generateTrees(chunk);
#else
  m_preloaded_generator.generateTrees(chunk);
#endif
#else
  m_procedural_generator.generateTrees(chunk);
#endif
}

bool TerrainGenerator::isChunkBelowOrAboveSurface(Chunk& chunk,
                                                  const HeightMap& height_map) {
  glm::ivec3 chunk_pos = chunk.getPos();
  LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
  return isChunkBelowOrAboveSurface(chunk_pos, height_map, lod);
}

bool TerrainGenerator::isChunkBelowOrAboveSurface(
    glm::ivec3 chunk_pos, const HeightMap& height_map,
    LevelOfDetail::LevelOfDetail lod) {
  // TODO: Calculate it once in HeightMapParser
  int block_amount_padding = lod.block_amount;
  double min_height = std::numeric_limits<double>::max();
  double max_height = std::numeric_limits<double>::min();
  for (int x = 1; x < block_amount_padding; x++) {
    for (int z = 1; z < block_amount_padding; z++) {
      min_height = std::min(min_height, height_map[x][z]);
      max_height = std::max(max_height, height_map[x][z]);
    }
  }
  // Real CHUNK_SIZE here is correct
  int chunk_pos_y = chunk_pos.y * CHUNK_SIZE;
  bool below_surface = chunk_pos_y + CHUNK_SIZE < min_height;
  bool above_surface = chunk_pos_y > max_height;
  return below_surface || above_surface;
}