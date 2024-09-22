#include "PaletteBlockStorage.h"


Block::PaletteBlockStorage::PaletteBlockStorage(uint8_t chunk_size, uint8_t initial_palettes_amount)
	: m_chunk_size{uint8_t(chunk_size)},
	m_index_storage{(uint8_t)std::max(static_cast<int>(log2(initial_palettes_amount)), 1), chunk_size}
{
	
	int chunk_size_cubed = m_chunk_size * m_chunk_size * m_chunk_size;

	if (initial_palettes_amount > 0) {
		m_palette.resize(1, { chunk_size_cubed, block_id::AIR });
	}
	if (initial_palettes_amount > 1) {
		m_palette.resize(2, { chunk_size_cubed, block_id::STONE });
	}
	if (initial_palettes_amount > 2) {
		m_palette.resize(3, { chunk_size_cubed, block_id::GRASS });
	}
	if (initial_palettes_amount > 3) {
		m_palette.resize(4, { chunk_size_cubed, block_id::SAND });
	}
	if (initial_palettes_amount > 4) {
		m_palette.resize(5, { chunk_size_cubed, block_id::DIRT });
	}
	m_occupancy_mask.resize(chunk_size_cubed, 0);
	int padded_chunk_size_cubed = (m_chunk_size + 2) * (m_chunk_size + 2) * (m_chunk_size + 2);
	m_padded_occupancy_mask.resize(padded_chunk_size_cubed, 0);
	m_padded_block_id_cache.resize(padded_chunk_size_cubed, block_id::AIR);
}

Block::PaletteBlockStorage::PaletteBlockStorage(LevelOfDetail::LevelOfDetail lod, uint8_t initial_palettes_amount)
	: PaletteBlockStorage(lod.block_amount, initial_palettes_amount)
{
}

Block::block_id Block::PaletteBlockStorage::get(glm::ivec3 block_pos)
{
	int block_index = getBlockIndex(block_pos);
	int bit_offset = block_index * m_index_storage.palette_index_size;
	uint8_t palette_index = m_index_storage.get(bit_offset);

	return m_palette.at(palette_index).block_type;
}

void Block::PaletteBlockStorage::set(glm::ivec3 block_pos, block_id block_type)
{
	int block_index = getBlockIndex(block_pos);
	m_occupancy_mask[block_index] = block_type != block_id::AIR;
	int padded_block_index = getPaddedBlockIndex(block_pos);
	m_padded_occupancy_mask[padded_block_index] = block_type != block_id::AIR;
	m_padded_block_id_cache[padded_block_index] = block_type;
	int bit_offset = block_index * m_index_storage.palette_index_size;
	uint8_t palette_index = m_index_storage.get(bit_offset);
	//LOG_F(INFO, "palette_index: %d, palette size: %d", palette_index, m_palette.size());
	PaletteEntry& current = m_palette[palette_index];

	current.refcount = std::max(current.refcount - 1, 0);

	int replace = findIndexOfPaletteHolding(static_cast<uint8_t>(block_type));

	// block_type is already in palette
	if (replace != -1)
	{
		m_index_storage.set(bit_offset, replace);
		m_palette[replace].refcount++;
		return;
	}

	// block_type is not in palette, but the current entry is empty
	if (current.refcount == 0)
	{
		current.block_type = static_cast<block_id>(block_type);
		current.refcount = 1;
		return;
	}
	LOG_F(WARNING, "(Costly) New palette needed for block type: %d", block_type);
	// create new palette entry
	uint8_t new_entry_index = newPaletteEntry();
	m_palette.at(new_entry_index) = PaletteEntry(1, block_type);
	// recalculate, because palette could grow, when newPaletteEntry() was called
	bit_offset = block_index * m_index_storage.palette_index_size;
	m_index_storage.set(bit_offset, new_entry_index);

}

bool Block::PaletteBlockStorage::isBlockPresent(glm::ivec3 block_pos)
{
	int block_index = getBlockIndex(block_pos);
	return m_occupancy_mask[block_index];
}

void Block::PaletteBlockStorage::clearIndexStorage()
{
	m_index_storage.indexes.clear();
	m_index_storage.palette_index_size = 1;
	m_index_storage.indexes_amount = 0;
}

void Block::PaletteBlockStorage::clear()
{
	clearIndexStorage();
	m_palette.clear();
	m_padded_block_id_cache.clear();
	m_padded_block_id_cache = std::move(std::vector<block_id>());
}

Block::PaletteIndexStorage& Block::PaletteBlockStorage::getPaletteIndexStorage()
{
	return m_index_storage;
}

sul::dynamic_bitset<>& Block::PaletteBlockStorage::getOccupancyMask()
{
	return m_occupancy_mask;
}

sul::dynamic_bitset<>& Block::PaletteBlockStorage::getPaddedOccupancyMask()
{
	return m_padded_occupancy_mask;
}

std::vector<Block::block_id>& Block::PaletteBlockStorage::getPaddedBlockIdCache()
{
	return m_padded_block_id_cache;
}

uint8_t Block::PaletteBlockStorage::newPaletteEntry()
{
	int first_free = findIndexOfPaletteHoldingOrEmpty(static_cast<uint8_t>(block_id::NONE));
	if (first_free != -1)
	{
		return first_free;
	}

	growPalette();
	return newPaletteEntry();
}

void Block::PaletteBlockStorage::growPalette()
{
	// 1. Decode indices
	std::vector<uint8_t> indices(m_index_storage.indexes_amount);
	uint32_t bit_offset = 0;
	for (uint32_t i = 0; i < m_index_storage.indexes_amount; i++)
	{
		bit_offset = i * m_index_storage.palette_index_size;
		indices[i] = m_index_storage.get(bit_offset);
	}

	// 2. Double palette entries
	PaletteEntry entry{ 0, block_id::NONE };
	m_palette.resize(m_palette.size() * 2, entry);
	
	// 3. Extend palette_index_size by one digit
	m_index_storage.palette_index_size = log2(m_palette.size());
	m_index_storage.indexes.clear();
	m_index_storage.indexes.resize(m_index_storage.indexes_amount * m_index_storage.palette_index_size);
	
	// 4. Encode indices
	for (uint32_t i = 0; i < m_index_storage.indexes_amount; i++)
	{
		bit_offset = i * m_index_storage.palette_index_size;
		m_index_storage.set(bit_offset, indices[i]);
	}
}

int Block::PaletteBlockStorage::findIndexOfPaletteHolding(uint8_t block_type)
{
	for (int i = 0; i < m_palette.size(); i++)
	{
		if (static_cast<uint8_t>(m_palette[i].block_type) == block_type)
		{
			return i;
		}
	}

	return -1;
}

int Block::PaletteBlockStorage::findIndexOfPaletteHoldingOrEmpty(uint8_t block_type)
{
	for (int i = 0; i < m_palette.size(); i++)
	{
		if (static_cast<uint8_t>(m_palette[i].block_type) == block_type || m_palette[i].refcount == 0)
		{
			return i;
		}
	}

	return -1;
}

int Block::PaletteBlockStorage::getBlockIndex(glm::ivec3 block_pos) const
{
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;
	return z + (x * m_chunk_size) + (y * m_chunk_size * m_chunk_size);
}

int Block::PaletteBlockStorage::getPaddedBlockIndex(glm::ivec3 block_pos) const
{
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;
	int padded_chunk_size = m_chunk_size + 2;
	return (z + 1) + ((x + 1) * padded_chunk_size) + ((y + 1) * padded_chunk_size * padded_chunk_size);
}