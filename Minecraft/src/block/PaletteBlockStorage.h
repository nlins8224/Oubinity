#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Block.h"
#include "../loguru.hpp"
#include "../level_of_detail/LevelOfDetail.h"
#include "../dynamic_bitset.hpp"


// credits to: https://www.reddit.com/r/VoxelGameDev/comments/9yu8qy/palettebased_compression_for_chunked_discrete/

namespace Block
{

struct PaletteEntry
{
	PaletteEntry(int refcount, block_id block_type)
	{
		this->refcount = refcount;
		this->block_type = block_type;
	}

	int refcount;
	block_id block_type;
};


// PaletteIndexStorage holds palette indices
// PaletteIndexStorage operates on bits.
struct PaletteIndexStorage
{
	PaletteIndexStorage(uint8_t index_size, uint32_t chunk_size)
	{
		this->palette_index_size = index_size;
		this->indexes_amount = chunk_size * chunk_size * chunk_size;
		indexes.resize(index_size * indexes_amount);
	}

	inline uint8_t get(uint32_t bit_offset) const
	{
		if (bit_offset + palette_index_size > indexes.size())
		{
			LOG_F(ERROR, "indexes size: %d, bit offset: %d, bit offset + palette_index_size: %d", indexes.size(), bit_offset, bit_offset + palette_index_size);
		}

		uint8_t palette_index = 0;
		for (uint32_t i = bit_offset; i < bit_offset + palette_index_size; i++)
		{
			palette_index |= indexes[i] << (i - bit_offset);
		}
		return palette_index;
	}

	inline void set(uint32_t bit_offset, uint8_t palette_index)
	{
		if (bit_offset + palette_index_size > indexes.size())
		{
			LOG_F(ERROR, "indexes size: %d, bit offset: %d, bit offset + palette_index_size: %d", indexes.size(), bit_offset, bit_offset + palette_index_size);
		}

		for (uint32_t i = bit_offset; i < bit_offset + palette_index_size; i++)
		{
			indexes[i] = palette_index & 1;
			palette_index >>= 1;
		}
	}

	sul::dynamic_bitset<>& data()
	{
		return indexes;
	}

	uint8_t getIndexSize()
	{
		return palette_index_size;
	}

	sul::dynamic_bitset<> indexes; 
	uint8_t palette_index_size; // in bits
	uint32_t indexes_amount;
};

class PaletteBlockStorage
{
public:
	PaletteBlockStorage(uint8_t chunk_size, uint8_t initial_palettes_amount = 1);
	PaletteBlockStorage(LevelOfDetail::LevelOfDetail lod, uint8_t initial_palettes_amount = 1);
	virtual ~PaletteBlockStorage() = default;
	block_id get(glm::ivec3 block_pos);
	void set(glm::ivec3 block_pos, block_id block_type);
	PaletteIndexStorage& getPaletteIndexStorage();
	sul::dynamic_bitset<>& getOccupancyMask();

private:
	uint8_t newPaletteEntry();
	void growPalette();
	int getBlockIndex(glm::ivec3 block_pos) const;
	int findIndexOfPaletteHolding(uint8_t block_type);
	int findIndexOfPaletteHoldingOrEmpty(uint8_t block_type);
	using Palette = std::vector<PaletteEntry>;
	Palette m_palette;
	PaletteIndexStorage m_index_storage;
	uint8_t m_padded_chunk_size;
	sul::dynamic_bitset<> m_occupancy_mask;
};

}