#include "PaletteBlockStorage.h"

Block::PaletteBlockStorage::PaletteBlockStorage(uint8_t chunk_size, uint8_t initial_palettes_amount)
	: m_chunk_size{chunk_size},
	m_index_storage{log2(initial_palettes_amount), chunk_size}
{
	PaletteEntry entry{ 0, block_id::NONE };
	m_palette.resize(1, entry);
	m_palette.at(0).block_type = block_id::AIR; // TODO: remove this, waste a space for a large number of chunklets
	m_palette.at(0).refcount = chunk_size * chunk_size * chunk_size; // TODO: remove this, waste a space for a large number of chunklets
}

Block::PaletteBlockStorage::PaletteBlockStorage(LevelOfDetail::LevelOfDetail lod, uint8_t initial_palettes_amount)
	: PaletteBlockStorage(lod.block_amount, initial_palettes_amount)
{
}

Block::block_id Block::PaletteBlockStorage::get(glm::ivec3 block_pos) const
{
	int block_index = getBlockIndex(block_pos);
	//LOG_F(INFO, "block_index: %d", block_index);
	int bit_offset = block_index * m_index_storage.palette_index_size;
	uint8_t palette_index = m_index_storage.get(bit_offset);
	//LOG_F(INFO, "palette_index: %d", palette_index);
	//LOG_F(INFO, "before m_palette.at, palette size: %d", m_palette.size());
	return m_palette.at(palette_index).block_type;
}

void Block::PaletteBlockStorage::set(glm::ivec3 block_pos, block_id block_type)
{
	int block_index = getBlockIndex(block_pos);
	int bit_offset = block_index * m_index_storage.palette_index_size;
	uint8_t palette_index = m_index_storage.get(bit_offset);
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

	// create new palette entry
	uint8_t new_entry_index = newPaletteEntry();
	//LOG_F(INFO, "new_entry_index: %d, block_type: %d, replace: %d", new_entry_index, static_cast<int>(block_type), replace);
	m_palette.at(new_entry_index) = PaletteEntry(1, block_type);
	// recalculate, because palette could grow, when newPaletteEntry() was called
	bit_offset = block_index * m_index_storage.palette_index_size;
	m_index_storage.set(bit_offset, new_entry_index);

}

void Block::PaletteBlockStorage::fill(block_id block_type)
{
	std::fill(m_index_storage.indexes.begin(), m_index_storage.indexes.end(), static_cast<uint8_t>(block_type));
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
	return block_pos.x * m_chunk_size * m_chunk_size + block_pos.y * m_chunk_size + block_pos.z;
}
