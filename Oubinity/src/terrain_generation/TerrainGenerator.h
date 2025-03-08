#pragma once
#include "../Settings.h"
#include "../chunk/Chunk.h"
#include "../tree/BranchGenerator.h"
#include "../tree/Tree.h"
#include "preloaded_generation/PreloadedGenerator.h"
#include "procedural_generation/ProceduralGenerator.h";

class TerrainGenerator {
 public:
  struct TreeModelSettings {
    int trees_amount;
    int crown_height;
    int crown_width;
    glm::ivec3 tree_pos;
  };
  TerrainGenerator() = delete;
  TerrainGenerator(int world_seed, uint8_t water_height);
  ~TerrainGenerator() = default;
  // true if chunk is visible, false if it's not
  bool generateChunkTerrain(Chunk& chunk);
  void generateChunkTerrain(Chunk& chunk, ProceduralHeightMap& height_map,
                            bool is_chunk_visible);
  void generateTrees(Chunk& chunk, ChunkSlidingWindow& chunk_sliding_window);
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
  std::vector<std::vector<ProceduralTree::Branch>> initTrees();
  std::vector<ProceduralTree::Branch> generateTree(TreeModelSettings tree_settings);
  std::vector<ProceduralTree::Branch>& chooseTree();
  void placeTrees(Chunk& chunk, HeightMap& surface_map,
                  TreePresenceMap& tree_presence_map, uint8_t water_height,
                  TreeModelSettings tree_settings,
                  ChunkSlidingWindow& chunk_sliding_window);

  ProceduralGenerator m_procedural_generator;
#if defined(SETTING_USE_PRELOADED_HEIGHTMAP) == 1 || \
    defined(SETTING_USE_PRELOADED_COLORMAP) == 1
  PreloadedGenerator m_preloaded_generator;
#endif
  uint8_t m_water_height;
  TreeModelSettings m_tree_settings;
  std::vector<std::vector<ProceduralTree::Branch>> m_trees;
  ProceduralTree::BranchGenerator m_branch_generator;
};
