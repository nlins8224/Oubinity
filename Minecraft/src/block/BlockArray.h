#pragma once
#include <glm/glm.hpp>
#include "Block.h"
#include "../chunk/ChunkSize.h"


namespace Block
{
class BlockArray
{
public:
	BlockArray(uint16_t block_amount);
	~BlockArray() = default;

	block_id get(glm::ivec3 block_pos) const;
	void set(glm::ivec3 block_pos, block_id block_type);
	void fill(block_id block_type);
private:
	int getBlockIndex(glm::ivec3 block_pos) const;
	bool outOfBounds(glm::ivec3 block_pos) const;
	bool outOfBounds(int coord, int block_amount) const;
	uint16_t m_block_amount;
	block_id block_array[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
};

}
