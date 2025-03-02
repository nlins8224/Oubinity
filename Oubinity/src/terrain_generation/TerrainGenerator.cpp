#include "TerrainGenerator.h"

#if defined(SETTING_USE_PRELOADED_HEIGHTMAP) == 1 || \
    defined(SETTING_USE_PRELOADED_COLORMAP) == 1
TerrainGenerator::TerrainGenerator(int world_seed, uint8_t water_height)
    : m_water_height{water_height},
      m_procedural_generator{ProceduralGenerator(world_seed, water_height)},
      m_preloaded_generator{PreloadedGenerator(water_height)} {

#if SETTING_TREES_ENABLED
  m_tree_settings = {.trees_amount = 10,
                     .crown_height = 3,
                     .crown_width = 5,
                     .tree_pos = glm::ivec3(0, 0, 0)};
  m_trees = initTrees();
#endif
}
#else
TerrainGenerator::TerrainGenerator(int world_seed, uint8_t water_height)
    : m_water_height{water_height},
      m_procedural_generator{ProceduralGenerator(world_seed, water_height)} {
#if SETTING_TREES_ENABLED
  m_tree_settings = {.trees_amount = 10,
                     .crown_height = 3,
                     .crown_width = 5,
                     .tree_pos = glm::ivec3(0, 0, 0)};
  m_trees = initTrees();
#endif
      }
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
    chunk.setChunkHasBlocksState(true);
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
  int block_amount_padding = lod.block_amount + 2;
  double min_height = std::numeric_limits<double>::max();
  double max_height = std::numeric_limits<double>::min();
  for (int x = 1; x < block_amount_padding - 1; x++) {
    for (int z = 1; z < block_amount_padding - 1; z++) {
      min_height = std::min(min_height, height_map[x][z]);
      max_height = std::max(max_height, height_map[x][z]);
    }
  }

  // Real CHUNK_SIZE here is correct
  int chunk_pos_y = chunk_pos.y * CHUNK_SIZE;
  // |CHUNK_SIZE * 2| we take into consideration one chunk that is below surface as well
  // to avoid empty voxel holes in surface that occur when surface height rises too quickly
  bool below_surface = chunk_pos_y + CHUNK_SIZE * 2 < min_height;
  bool above_surface = chunk_pos_y > max_height;
  return below_surface || above_surface;
}

void TerrainGenerator::placeTrees(Chunk& chunk, HeightMap& surface_map,
                                    TreePresenceMap& tree_presence_map,
                                    uint8_t water_height,
                                    TreeSettings tree_settings, ChunkSlidingWindow& chunk_sliding_window) {
  glm::ivec3 chunk_pos = chunk.getPos();
  LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
  int chunk_world_y = chunk_pos.y * lod.block_amount;

  for (uint8_t x = 0; x < lod.block_amount; x++) {
    for (uint8_t z = 0; z < lod.block_amount; z++) {
      if (tree_presence_map[x][z]) {
        uint8_t crown_height = tree_settings.crown_height;
        uint8_t crown_width = tree_settings.crown_width;
        crown_height += crown_height % 2 == 0;  // round to odd
        crown_width += crown_width % 2 == 0;    // round to odd

        Tree tree{crown_height, crown_width};
        uint8_t tree_plant_height =
            static_cast<uint8_t>(surface_map[x][z]) % lod.block_amount;
        if (chunk.getBlockId({x, tree_plant_height, z}) == Block::GRASS &&
            surface_map[x][z] >= water_height) {
          tree.spawnTree(chunk, chooseTree(),
                         glm::ivec3(x, tree_plant_height, z),
                         chunk_sliding_window);
        }
      }
    }
  }
}

std::vector<std::vector<ProceduralTree::Branch>> TerrainGenerator::initTrees() {
  std::vector<std::vector<ProceduralTree::Branch>> trees;
  for (int i = 0; i < m_tree_settings.trees_amount; i++) {
    trees.emplace_back(generateTree(m_tree_settings));
  }
  return trees;
}

std::vector<ProceduralTree::Branch> TerrainGenerator::generateTree(
    TreeSettings tree_settings) {
  return m_branch_generator.generateBranches(tree_settings.tree_pos);
}

std::vector<ProceduralTree::Branch>& TerrainGenerator::chooseTree() {
  CHECK_GT_F(m_trees.size(), 0, "Tree array was not inited correctly");
  std::random_device rand_dev;
  std::mt19937 generator(rand_dev());
  std::uniform_int_distribution<int> distr(0, m_trees.size() - 1);
  return m_trees.at(distr(generator));
}

void TerrainGenerator::generateTrees(Chunk& chunk,
                                     ChunkSlidingWindow& chunk_sliding_window) {
  glm::ivec3 chunk_pos = chunk.getPos();
#if SETTING_USE_HEIGHTMAP_BLENDING
  HeightMap height_map =
      generateBlendedHeightMap(chunk.getPos(), chunk.getLevelOfDetail());
#elif SETTING_USE_PRELOADED_HEIGHTMAP
  HeightMap height_map =
      m_preloaded_generator.getHeightMap(chunk_pos, chunk.getLevelOfDetail());
#else
  HeightMap height_map = m_procedural_generator.generateHeightMap(chunk);
#endif
  TreePresenceMap tree_presence_map =
      m_preloaded_generator.generateTreePresenceMap(
          m_preloaded_generator.getTreeMap(chunk_pos));
  placeTrees(chunk, height_map, tree_presence_map, m_water_height,
             m_tree_settings, chunk_sliding_window);
}
