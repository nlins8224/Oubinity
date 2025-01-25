#include "BlockStorage.h"

Block::BlockStorage::BlockStorage(uint8_t chunk_size,
                                  uint8_t initial_palettes_amount)
    : m_chunk_size{uint8_t(chunk_size)} {
  int chunk_size_cubed = m_chunk_size * m_chunk_size * m_chunk_size;
  int padded_chunk_size_cubed =
      (m_chunk_size + 2) * (m_chunk_size + 2) * (m_chunk_size + 2);
  m_padded_occupancy_mask.resize(padded_chunk_size_cubed, 0);
  m_padded_block_id_cache.resize(padded_chunk_size_cubed, block_id::AIR);
}

Block::BlockStorage::BlockStorage(LevelOfDetail::LevelOfDetail lod,
                                  uint8_t initial_palettes_amount)
    : BlockStorage(lod.block_amount, initial_palettes_amount) {}

Block::block_id Block::BlockStorage::getRaw(glm::ivec3 block_padded_pos) {
  int padded_block_index = getBlockIndex(block_padded_pos);
  return m_padded_block_id_cache[padded_block_index];
}

void Block::BlockStorage::setRaw(glm::ivec3 block_padded_pos,
                                 block_id block_type) {
  int padded_block_index = getBlockIndex(block_padded_pos);
  if (padded_block_index > m_padded_block_id_cache.size()) {
    LOG_F(ERROR,
          "padded_block_index size: %d > m_padded_block_id_cache size: %d ",
          padded_block_index, m_padded_block_id_cache.size());
    m_padded_block_id_cache.resize((m_chunk_size + 2) * (m_chunk_size + 2) * (m_chunk_size + 2));
  }
  m_padded_occupancy_mask[padded_block_index] = block_type != block_id::AIR;
  m_padded_block_id_cache.at(padded_block_index) = block_type;
}

bool Block::BlockStorage::isBlockPresent(glm::ivec3 block_padded_pos) {
  int block_index = getBlockIndex(block_padded_pos);
  return m_padded_occupancy_mask[block_index] != block_id::AIR;
}

void Block::BlockStorage::clearBlockIdCache() {
  m_padded_block_id_cache.clear();
  m_padded_block_id_cache = std::move(std::vector<block_id>());
}

void Block::BlockStorage::resizeIfNeeded() { 
   int padded_chunk_size_cubed = (m_chunk_size + 2) * (m_chunk_size + 2) * (m_chunk_size + 2);
   if (m_padded_block_id_cache.size() < padded_chunk_size_cubed) {
     m_padded_block_id_cache.resize(padded_chunk_size_cubed, block_id::AIR);
   }
}

sul::dynamic_bitset<>& Block::BlockStorage::getPaddedOccupancyMask() {
  return m_padded_occupancy_mask;
}

std::vector<Block::block_id>& Block::BlockStorage::getPaddedBlockIdCache() {
  return m_padded_block_id_cache;
}

int Block::BlockStorage::getBlockIndex(glm::ivec3 block_padded_pos) const {
  int x = block_padded_pos.x, y = block_padded_pos.y, z = block_padded_pos.z;
  int padded_chunk_size = m_chunk_size + 2;
  return z + (x * padded_chunk_size) + (y * padded_chunk_size * padded_chunk_size);
}