#include "BlockArray.h"
namespace Block
{
	block_id BlockArray::get(glm::ivec3 block_pos) const
	{
		return block_array[getBlockIndex(block_pos)];
	}

	void BlockArray::set(glm::ivec3 block_pos, block_id block_type)
	{
		block_array[getBlockIndex(block_pos)] = block_type;
	}

	int BlockArray::getBlockIndex(glm::ivec3 block_pos) const
	{
		return block_pos.x * CHUNK_SIZE * CHUNK_SIZE + block_pos.y * CHUNK_SIZE + block_pos.z;
	}

	void BlockArray::fill(block_id block_type)
	{
		memset(block_array, block_type, sizeof(block_array));
	}
}
