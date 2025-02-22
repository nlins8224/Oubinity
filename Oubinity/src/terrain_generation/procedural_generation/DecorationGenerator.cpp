#include "DecorationGenerator.h"

DecorationGenerator::DecorationGenerator() {
  m_tree_shape.crown_height = 3;
  m_tree_shape.crown_width = 5;
  m_tree_height_boundaries.height_min = 0;
  m_tree_height_boundaries.height_max = 9;
}

void DecorationGenerator::placeTrees(Chunk& chunk, HeightMap& surface_map,
                                        TreePresenceMap& tree_presence_map,
                                        uint8_t water_height) {

}

void DecorationGenerator::generateTrees(Chunk& chunk, HeightMap& surface_map,
                                        TreePresenceMap& tree_presence_map,
                                        uint8_t water_height) {
}
