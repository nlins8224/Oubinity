#include "BlockStorage.h"


Block::BlockStorage::BlockStorage(uint8_t chunk_size, uint8_t initial_palettes_amount)
	: m_chunk_size{uint8_t(chunk_size)}
{
	
	int chunk_size_cubed = m_chunk_size * m_chunk_size * m_chunk_size;
	m_occupancy_mask.resize(chunk_size_cubed, 0);
	int padded_chunk_size_cubed = (m_chunk_size + 2) * (m_chunk_size + 2) * (m_chunk_size + 2);
	m_padded_occupancy_mask.resize(padded_chunk_size_cubed, 0);
	m_padded_block_id_cache.resize(padded_chunk_size_cubed, block_id::AIR);
}

Block::BlockStorage::BlockStorage(LevelOfDetail::LevelOfDetail lod, uint8_t initial_palettes_amount)
	: BlockStorage(lod.block_amount, initial_palettes_amount)
{
}

Block::block_id Block::BlockStorage::getRaw(glm::ivec3 block_pos)
{
	int padded_block_index = getPaddedBlockIndex(block_pos);
	return m_padded_block_id_cache[padded_block_index];
}

void Block::BlockStorage::setRaw(glm::ivec3 block_pos, block_id block_type)
{
	int block_index = getBlockIndex(block_pos);
	m_occupancy_mask[block_index] = block_type != block_id::AIR;
	int padded_block_index = getPaddedBlockIndex(block_pos);
	m_padded_occupancy_mask[padded_block_index] = block_type != block_id::AIR;
	m_padded_block_id_cache[padded_block_index] = block_type;
}

bool Block::BlockStorage::isBlockPresent(glm::ivec3 block_pos)
{
	int block_index = getPaddedBlockIndex(block_pos);
	return m_padded_occupancy_mask[block_index];
}

void Block::BlockStorage::clear()
{
	m_padded_block_id_cache.clear();
	m_padded_block_id_cache = std::move(std::vector<block_id>());
}

sul::dynamic_bitset<>& Block::BlockStorage::getOccupancyMask()
{
	return m_occupancy_mask;
}

sul::dynamic_bitset<>& Block::BlockStorage::getPaddedOccupancyMask()
{
	return m_padded_occupancy_mask;
}

std::vector<Block::block_id>& Block::BlockStorage::getPaddedBlockIdCache()
{
	return m_padded_block_id_cache;
}

int Block::BlockStorage::getBlockIndex(glm::ivec3 block_pos) const
{
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;
	return z + (x * m_chunk_size) + (y * m_chunk_size * m_chunk_size);
}

int Block::BlockStorage::getPaddedBlockIndex(glm::ivec3 block_pos) const
{
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;
	int padded_chunk_size = m_chunk_size + 2;
	return (z + 1) + ((x + 1) * padded_chunk_size) + ((y + 1) * padded_chunk_size * padded_chunk_size);
}