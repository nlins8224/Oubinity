#pragma once
#include <vector>
#include "../../Settings.h"
#include "../../chunk/Chunk.h"
#include "../TerrainGenerationTypes.h"
#include "../procedural_generation/DecorationGenerator.h"
#include "FileMapParser.h"

class PreloadedGenerator {
 public:
  PreloadedGenerator(uint8_t water_height,
                     glm::vec3 scale = Settings::SETTING_SCALE);
  virtual ~PreloadedGenerator() = default;
  bool generateLayers(Chunk& chunk, const HeightMap& height_map,
                      const BlockMap& block_map);
  HeightMap getHeightMap(glm::ivec3 chunk_pos,
                         LevelOfDetail::LevelOfDetail lod);
  BlockMap getBlockMap(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod);
  void generateTrees(Chunk& chunk);

 private:
  HeightMap& getTreeMap(glm::ivec3 chunk_pos);
  TreePresenceMap generateTreePresenceMap(HeightMap& tree_map);
  bool isBlockInSurfaceHeightBounds(glm::ivec3 block_pos, glm::ivec3 chunk_pos,
                                    int surface_height, int block_size);
  bool isBlockUnderneathSurface(glm::ivec3 block_pos, glm::ivec3 chunk_pos,
                                int surface_height, int block_size);
  glm::ivec3 mapChunkPosToHeightMapPos(glm::ivec3 chunk_pos);

  std::vector<HeightMap> m_height_maps;
  std::vector<BlockMap> m_block_maps;
  std::vector<HeightMap> m_tree_maps;

  uint32_t m_chunks_in_heightmap_xz;
  uint32_t m_chunks_in_blockmap_xz;

  uint8_t m_water_height;
  DecorationGenerator m_decoration_generator;
};