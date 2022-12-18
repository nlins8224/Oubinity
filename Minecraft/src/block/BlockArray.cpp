#include "BlockArray.h"
namespace Block
{
	block_id BlockArray::get(glm::ivec3 block_pos)
	{
		if (outOfBounds(block_pos))
			return block_id::AIR;

		return block_array[getBlockIndex(block_pos)];
	}

	void BlockArray::set(glm::ivec3 block_pos, block_id block_type)
	{
		if (outOfBounds(block_pos))
			return;

		block_array[getBlockIndex(block_pos)] = block_type;
	}

	int BlockArray::getBlockIndex(glm::ivec3 block_pos)
	{
		return block_pos.x * Chunk::CHUNK_SIZE_Y * Chunk::CHUNK_SIZE_Z + block_pos.y * Chunk::CHUNK_SIZE_Z + block_pos.z;
	}

	bool BlockArray::outOfBounds(glm::ivec3 block_pos)
	{
		return outOfBounds(block_pos.x, Chunk::CHUNK_SIZE_X) 
			|| outOfBounds(block_pos.y, Chunk::CHUNK_SIZE_Y)
			|| outOfBounds(block_pos.z, Chunk::CHUNK_SIZE_Z);
	}

	bool BlockArray::outOfBounds(int coord, int chunk_size)
	{
		return coord < 0 || coord > chunk_size;
	}

}
