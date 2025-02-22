#include "PreloadedGenerator.h"
#include "../../Util.h"
#include "../procedural_generation/ProceduralGenerator.h"

PreloadedGenerator::PreloadedGenerator(uint8_t water_height, glm::vec3 scale)
    : m_water_height{water_height} {
  HeightMapBundle height_map_bundle =
      PreloadedGeneration::parsePNGToHeightMaps_8BIT("assets/canyon_heightmap.png",
                                                     scale);
  BlockMapBundle block_map_bundle = PreloadedGeneration::parsePNGToBlockMaps(
      "assets/canyon_colormap.png", scale);
  HeightMapBundle tree_map_bundle =
      PreloadedGeneration::parsePNGToHeightMaps_8BIT(
          "assets/treemap.png", scale);

  m_height_maps =
      std::vector(height_map_bundle.maps.begin(), height_map_bundle.maps.end());
  m_block_maps =
      std::vector(block_map_bundle.maps.begin(), block_map_bundle.maps.end());
  m_tree_maps =
      std::vector(tree_map_bundle.maps.begin(), tree_map_bundle.maps.end());

  m_chunks_in_heightmap_xz = height_map_bundle.world_width / CHUNK_SIZE;
  m_chunks_in_blockmap_xz = block_map_bundle.world_width / CHUNK_SIZE;
  m_tree_settings = {.trees_amount = 10,
                     .crown_height = 3,
                     .crown_width = 5,
                     .tree_pos = glm::ivec3(0, 0, 0)};
  m_trees = initTrees();
}

HeightMap PreloadedGenerator::getHeightMap(glm::ivec3 chunk_pos,
                                           LevelOfDetail::LevelOfDetail lod) {
  glm::ivec3 chunk_pos_in_heightmap = mapChunkPosToHeightMapPos(chunk_pos);
  HeightMap height_map =
      m_height_maps.at(chunk_pos_in_heightmap.x * m_chunks_in_heightmap_xz +
                       chunk_pos_in_heightmap.z);
  return PreloadedGeneration::increaseHeightMapLodLevel(height_map, lod);
}

BlockMap PreloadedGenerator::getBlockMap(glm::ivec3 chunk_pos,
                                         LevelOfDetail::LevelOfDetail lod) {
  glm::ivec3 chunk_pos_in_map = mapChunkPosToHeightMapPos(chunk_pos);
  BlockMap color_map = m_block_maps.at(
      chunk_pos_in_map.x * m_chunks_in_blockmap_xz + chunk_pos_in_map.z);
  return PreloadedGeneration::increaseBlockMapLodLevel(color_map, lod);
}

HeightMap& PreloadedGenerator::getTreeMap(glm::ivec3 chunk_pos) {
  glm::ivec3 chunk_pos_in_map = mapChunkPosToHeightMapPos(chunk_pos);
  return m_tree_maps.at(chunk_pos_in_map.x * m_chunks_in_heightmap_xz +
                        chunk_pos_in_map.z);
}

TreePresenceMap PreloadedGenerator::generateTreePresenceMap(
    HeightMap& tree_map) {
  TreePresenceMap tree_presence_map{};
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int z = 0; z < CHUNK_SIZE; z++) {
      tree_presence_map[x][z] = tree_map[x][z] > 0.0;
    }
  }
  return tree_presence_map;
}

bool PreloadedGenerator::generateLayers(Chunk& chunk,
                                        const HeightMap& height_map,
                                        const BlockMap& block_map) {
  int block_amount_padding = chunk.getLevelOfDetail().block_amount + 2;
  int block_size = chunk.getLevelOfDetail().block_size;
  bool anything_added = false;
  glm::ivec3 chunk_world_pos = chunk.getPos() * CHUNK_SIZE;

  for (int x = 0; x < block_amount_padding; x++) {
    for (int z = 0; z < block_amount_padding; z++) {
      // TODO: y for loop should not be needed at all.
      for (int y = 1; y < block_amount_padding - 1; y++) {
        glm::ivec3 block_pos{x, y, z};
        float surface_height = height_map[x][z];
        glm::ivec3 block_padded_pos =
            chunk_world_pos + (block_pos * block_size);
        // Y axis cuts through Y Chunk column. There is one surface and one
        // chunk that it should cut. Do not adjust Y block coord here.
        glm::ivec3 block_world_pos =
            block_padded_pos - glm::ivec3(1, 0, 1) * block_size;

        if (surface_height > block_world_pos.y - block_size &&
            surface_height < block_world_pos.y + block_size) {
          block_id block = block_map[block_pos.x][block_pos.z];
          chunk.setBlock(block_pos, block);
          anything_added = true;
        } else if (surface_height >= block_world_pos.y + block_size) {
          chunk.setBlock(block_pos, Block::STONE);
          anything_added = true;
        }
      }
    }
  }
  return anything_added;
}

bool PreloadedGenerator::isBlockInSurfaceHeightBounds(glm::ivec3 block_pos,
                                                      glm::ivec3 chunk_pos,
                                                      int surface_height,
                                                      int block_size) {
  glm::ivec3 block_world_pos =
      chunk_pos * CHUNK_SIZE + (block_pos * block_size);
  int max_surface_height = block_world_pos.y + block_size;
  int min_surface_height =
      block_world_pos.y - (block_size * Settings::SETTING_BLOCK_MARGIN);
  return surface_height >= min_surface_height &&
         surface_height <= max_surface_height;
}

bool PreloadedGenerator::isBlockUnderneathSurface(glm::ivec3 block_pos,
                                                  glm::ivec3 chunk_pos,
                                                  int surface_height,
                                                  int block_size) {
  glm::ivec3 block_world_pos =
      chunk_pos * CHUNK_SIZE + (block_pos * block_size);
  return block_world_pos.y < surface_height;
}

glm::ivec3 PreloadedGenerator::mapChunkPosToHeightMapPos(glm::ivec3 chunk_pos) {
  glm::ivec3 translated_chunk_pos =
      chunk_pos + (((int)m_chunks_in_heightmap_xz - 1) / 2);
  int x{translated_chunk_pos.x}, y{translated_chunk_pos.y},
      z{translated_chunk_pos.z};
  glm::ivec3 target_chunk_pos = {Util::getMod(x, m_chunks_in_heightmap_xz),
                                 chunk_pos.y,
                                 Util::getMod(z, m_chunks_in_heightmap_xz)};
  return target_chunk_pos;
}

void PreloadedGenerator::placeTrees(Chunk& chunk, HeightMap& surface_map,
                                    TreePresenceMap& tree_presence_map,
                                    uint8_t water_height, TreeSettings tree_settings) {

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
            tree_plant_height > water_height && z % 10 == 0 && x % 10 == 0) {
          tree.spawnTree(chunk, chooseTree(), glm::ivec3(x, tree_plant_height, z));
        }
      }
    }
  }

}

std::vector<std::vector<ProceduralTree::Branch>>
PreloadedGenerator::initTrees() {
  std::vector<std::vector<ProceduralTree::Branch>> trees;
  for (int i = 0; i < m_tree_settings.trees_amount; i++) {
    trees.emplace_back(generateTree(m_tree_settings));
  }
  return trees;
}

std::vector<ProceduralTree::Branch> PreloadedGenerator::generateTree(TreeSettings tree_settings) {
  return m_branch_generator.generateBranches(tree_settings.tree_pos);
}

std::vector<ProceduralTree::Branch>& PreloadedGenerator::chooseTree() {
  CHECK_GT_F(m_trees.size(), 0, "Tree array was not inited correctly");
  std::random_device rand_dev;
  std::mt19937 generator(rand_dev());
  std::uniform_int_distribution<int> distr(0, m_trees.size() - 1);
  return m_trees.at(distr(generator));
}

void PreloadedGenerator::generateTrees(Chunk& chunk) {
   glm::ivec3 chunk_pos = chunk.getPos();
   HeightMap height_map = getHeightMap(chunk_pos, chunk.getLevelOfDetail());
   TreePresenceMap tree_presence_map =
   generateTreePresenceMap(getTreeMap(chunk_pos));
   placeTrees(chunk, height_map, tree_presence_map,
   m_water_height, m_tree_settings);
}
