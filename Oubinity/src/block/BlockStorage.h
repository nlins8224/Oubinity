#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "../dynamic_bitset.hpp"
#include "../level_of_detail/LevelOfDetail.h"
#include "../loguru.hpp"
#include "Block.h"

namespace Block {

class BlockStorage {
 public:
  BlockStorage(uint8_t chunk_size, uint8_t initial_palettes_amount = 8);
  BlockStorage(LevelOfDetail::LevelOfDetail lod,
               uint8_t initial_palettes_amount = 8);
  virtual ~BlockStorage() = default;
  block_id getRaw(glm::ivec3 block_padded_pos);
  void setRaw(glm::ivec3 block_padded_pos, block_id block_type);
  bool isBlockPresent(glm::ivec3 block_padded_pos);
  void clearBlockIdCache();
  void resizeIfNeeded();
  sul::dynamic_bitset<>& getPaddedOccupancyMask();
  std::vector<block_id>& getPaddedBlockIdCache();

 private:
  int getBlockIndex(glm::ivec3 block_padded_pos) const;
  uint8_t m_chunk_size;
  sul::dynamic_bitset<> m_padded_occupancy_mask;
  std::vector<block_id> m_padded_block_id_cache;
};

}  // namespace Block