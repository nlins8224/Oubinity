#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Block.h"
#include "../loguru.hpp"

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


// BlockStorage holds palette indices
// BlockStorage operates on bits.
struct BlockStorage
{
	BlockStorage(uint8_t block_size, uint32_t chunk_size)
	{
		this->block_size = block_size;
		this->block_amount = chunk_size * chunk_size * chunk_size;
		blocks.resize(block_size * block_amount);
	}

	inline uint8_t get(uint32_t bit_offset) const
	{
		if (bit_offset + block_size > blocks.size())
		{
			LOG_F(ERROR, "blocks size: %d, bit offset: %d, bit offset + block_size: %d", blocks.size(), bit_offset, bit_offset + block_size);
		}

		uint8_t palette_index = 0;
		for (uint32_t i = bit_offset; i < bit_offset + block_size; i++)
		{
			palette_index |= blocks[i] << (i - bit_offset);
		}
		return palette_index;
	}

	inline void set(uint32_t bit_offset, uint8_t palette_index)
	{
		if (bit_offset + block_size > blocks.size())
		{
			LOG_F(ERROR, "blocks size: %d, bit offset: %d, bit offset + block_size: %d", blocks.size(), bit_offset, bit_offset + block_size);
		}

		for (uint32_t i = bit_offset; i < bit_offset + block_size; i++)
		{
			blocks[i] = palette_index & 1;
			palette_index >>= 1;
		}
	}

	// vector<bool> is used as dynamic bitset, as there is no available dynamic bitset data structure in the standard lib.
    // Note that vector<bool> specialization does not met container criteria (by STL's definition of what container is).
    // https://en.cppreference.com/w/cpp/container/vector_bool
	std::vector<bool> blocks; 
	uint8_t block_size; // in bits
	uint32_t block_amount;
};

class PaletteBlockStorage
{
public:
	PaletteBlockStorage(uint8_t chunk_size, uint8_t initial_palettes_amount = 2);
	virtual ~PaletteBlockStorage() = default;
	block_id get(glm::ivec3 block_pos) const;
	void set(glm::ivec3 block_pos, block_id block_type);
private:
	uint8_t newPaletteEntry();
	void growPalette();
	int getBlockIndex(glm::ivec3 block_pos) const;
	int findIndexOfPaletteHolding(uint8_t block_type);
	using Palette = std::vector<PaletteEntry>;
	Palette m_palette;
	BlockStorage m_block_storage;
	uint8_t m_chunk_size;

	void encodeDecodeTest();
};

}