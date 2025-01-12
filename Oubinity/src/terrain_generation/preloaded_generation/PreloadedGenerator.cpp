#include "PreloadedGenerator.h"
#include "../../Util.h"

PreloadedGenerator::PreloadedGenerator(uint8_t water_height, glm::vec3 scale)
    : m_water_height{water_height} {
  HeightMapBundle height_map_bundle =
      PreloadedGeneration::parsePNGToHeightMaps_8BIT("assets/canyon_heightmap.png",
                                                     scale);
  BlockMapBundle block_map_bundle = PreloadedGeneration::parsePNGToBlockMaps(
      "assets/canyon_colormap.png", scale);
  HeightMapBundle tree_map_bundle =
      PreloadedGeneration::parsePNGToHeightMaps_8BIT(
          "assets/gaea31_treemap.png", scale);

  m_height_maps =
      std::vector(height_map_bundle.maps.begin(), height_map_bundle.maps.end());
  m_block_maps =
      std::vector(block_map_bundle.maps.begin(), block_map_bundle.maps.end());
  m_tree_maps =
      std::vector(tree_map_bundle.maps.begin(), tree_map_bundle.maps.end());

  m_chunks_in_heightmap_xz = height_map_bundle.world_width / CHUNK_SIZE;
  m_chunks_in_blockmap_xz = block_map_bundle.world_width / CHUNK_SIZE;
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
      tree_presence_map[x][z] = tree_map[x][z] > 0.5;
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
      for (int y = 0; y < block_amount_padding; y++) {
        glm::ivec3 block_pos{x, y, z};
        float surface_height = height_map[x][z];
        glm::ivec3 block_padded_pos =
            chunk_world_pos + (block_pos * block_size);
        glm::ivec3 block_world_pos =
            block_padded_pos - glm::ivec3(1, 1, 1) * block_size;

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
  // LOG_F(WARNING, "chunk_pos: (%d, %d, %d)", chunk_pos.x, chunk_pos.y,
  // chunk_pos.z); LOG_F(WARNING, "translated_chunk_pos: (%d, %d, %d)",
  // translated_chunk_pos.x, translated_chunk_pos.y, translated_chunk_pos.z);

  int x{translated_chunk_pos.x}, y{translated_chunk_pos.y},
      z{translated_chunk_pos.z};
  glm::ivec3 target_chunk_pos = {Util::getMod(x, m_chunks_in_heightmap_xz),
                                 chunk_pos.y,
                                 Util::getMod(z, m_chunks_in_heightmap_xz)};
  // LOG_F(WARNING, "target_chunk_pos: (%d, %d, %d)", target_chunk_pos.x,
  // target_chunk_pos.y, target_chunk_pos.z);
  return target_chunk_pos;
}

void PreloadedGenerator::generateTrees(Chunk& chunk) {
   glm::ivec3 chunk_pos = chunk.getPos();
   HeightMap height_map = getHeightMap(chunk_pos, chunk.getLevelOfDetail());
   TreePresenceMap tree_presence_map =
   generateTreePresenceMap(getTreeMap(chunk_pos));
   m_decoration_generator.generateTrees(chunk, height_map, tree_presence_map,
   m_water_height);
}
