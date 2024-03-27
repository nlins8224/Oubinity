#include "PaletteBlockStorage.h"

Block::PaletteBlockStorage::PaletteBlockStorage(uint8_t chunk_size, uint8_t initial_palettes_amount)
	: m_chunk_size{chunk_size},
	m_block_storage{log2(initial_palettes_amount), chunk_size}
{
	PaletteEntry entry{ 0, block_id::NONE };
	m_palette.resize(initial_palettes_amount, entry);
	m_palette.at(0).block_type = block_id::AIR;
	m_palette.at(0).refcount = chunk_size * chunk_size * chunk_size;

	//m_palette.at(1).block_type = block_id::STONE;
	//m_palette.at(1).refcount = 0;

	//m_palette.at(2).block_type = block_id::GRASS;
	//m_palette.at(2).refcount = 0;

	//m_palette.at(3).block_type = block_id::OAK_LOG;
	//m_palette.at(3).refcount = 0;

	//m_palette.at(4).block_type = block_id::OAK_LEAVES;
	//m_palette.at(4).refcount = 0;
}

Block::PaletteBlockStorage::PaletteBlockStorage(LevelOfDetail::LevelOfDetail lod, uint8_t initial_palettes_amount)
	: PaletteBlockStorage(lod.block_amount, initial_palettes_amount)
{
}

Block::block_id Block::PaletteBlockStorage::get(glm::ivec3 block_pos) const
{
	int block_index = getBlockIndex(block_pos);
	int bit_offset = block_index * m_block_storage.block_size;
	uint8_t palette_index = m_block_storage.get(bit_offset);
	return m_palette.at(palette_index).block_type;
}

void Block::PaletteBlockStorage::set(glm::ivec3 block_pos, block_id block_type)
{
	int block_index = getBlockIndex(block_pos);
	int bit_offset = block_index * m_block_storage.block_size;
	uint8_t palette_index = m_block_storage.get(bit_offset);
	PaletteEntry& current = m_palette[palette_index];

	current.refcount = std::max(current.refcount - 1, 0);

	int replace = findIndexOfPaletteHolding(static_cast<uint8_t>(block_type));
	//LOG_F(INFO, "palette index: %d", replace);

	// block_type is already in palette
	if (replace != -1)
	{
		//LOG_F(INFO, "block in palette");
		m_block_storage.set(bit_offset, replace);
		m_palette[replace].refcount++;
		return;
	}

	// block_type is not in palette, but the current entry is empty
	if (current.refcount == 0)
	{
		LOG_F(INFO, "block not in palette, but current one is empty");
		//m_block_storage.set(bit_offset, replace);
		current.block_type = static_cast<block_id>(block_type);
		current.refcount = 1;
		return;
	}

	// create new palette entry
	//LOG_F(INFO, "new palette entry");
	uint8_t new_entry_index = newPaletteEntry();
	//LOG_F(INFO, "new_entry_index: %d, block_type: %d, replace: %d", new_entry_index, static_cast<int>(block_type), replace);
	m_palette.at(new_entry_index) = PaletteEntry(1, block_type);
	// recalculate, because palette could grow, when newPaletteEntry() was called
	bit_offset = block_index * m_block_storage.block_size;
	m_block_storage.set(bit_offset, new_entry_index);

}

void Block::PaletteBlockStorage::fill(block_id block_type)
{
	std::fill(m_block_storage.blocks.begin(), m_block_storage.blocks.end(), static_cast<uint8_t>(block_type));
}

uint8_t Block::PaletteBlockStorage::newPaletteEntry()
{
	int first_free = findIndexOfPaletteHolding(static_cast<uint8_t>(block_id::NONE));
	if (first_free != -1)
	{
		//LOG_F(INFO, "first_free: %d", first_free);
		return first_free;
	}

	growPalette();
	return newPaletteEntry();
}

void Block::PaletteBlockStorage::growPalette()
{
	//LOG_F(INFO, "growPalette()");
	// 1. Decode indices
	std::vector<uint8_t> indices(m_block_storage.block_amount);
	uint32_t bit_offset = 0;
	for (uint32_t i = 0; i < m_block_storage.block_amount; i++)
	{
		bit_offset = i * m_block_storage.block_size;
		indices[i] = m_block_storage.get(bit_offset);
	}

	// 2. Double palette entries
	PaletteEntry entry{ 0, block_id::NONE };
	m_palette.resize(m_palette.size() * 2, entry);
	
	// 3. Extend block_size by one digit
	m_block_storage.block_size = log2(m_palette.size());
	m_block_storage.blocks.clear();
	m_block_storage.blocks.resize(m_block_storage.block_amount * m_block_storage.block_size);
	
	// 4. Encode indices
	for (uint32_t i = 0; i < m_block_storage.block_amount; i++)
	{
		bit_offset = i * m_block_storage.block_size;
		m_block_storage.set(bit_offset, indices[i]);
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

int Block::PaletteBlockStorage::getBlockIndex(glm::ivec3 block_pos) const
{
	return block_pos.x * m_chunk_size * m_chunk_size + block_pos.y * m_chunk_size + block_pos.z;
}
