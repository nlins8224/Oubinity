#include "DecorationGenerator.h"

DecorationGenerator::DecorationGenerator() {
  m_tree_shape.crown_height = 3;
  m_tree_shape.crown_width = 5;
  m_tree_height_boundaries.height_min = 0;
  m_tree_height_boundaries.height_max = 9;
}

void DecorationGenerator::generateTrees(Chunk& chunk, HeightMap& surface_map,
                                        TreePresenceMap& tree_presence_map,
                                        uint8_t water_height) {
  glm::ivec3 chunk_pos = chunk.getPos();
  LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
  int chunk_world_y = chunk_pos.y * lod.block_amount;

  for (uint8_t x = 0; x < lod.block_amount; x++) {
    for (uint8_t z = 0; z < lod.block_amount; z++) {
      if (tree_presence_map[x][z]) {
        uint8_t crown_height = m_tree_shape.crown_height;
        uint8_t crown_width = m_tree_shape.crown_width;
        crown_height += crown_height % 2 == 0;  // round to odd
        crown_width += crown_width % 2 == 0;    // round to odd

        Tree tree{crown_height, crown_width};
        uint8_t tree_plant_height =
            static_cast<uint8_t>(surface_map[x][z]) % lod.block_amount;
        if (chunk.getBlockId({x, tree_plant_height, z}) == Block::GRASS &&
            tree_plant_height > water_height && (z + x) % 2 == 0) {
          tree.addTree(chunk, {x, tree_plant_height, z});
        }
      }
    }
  }
}
