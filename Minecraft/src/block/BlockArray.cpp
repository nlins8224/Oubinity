#include "BlockArray.h"
namespace Block
{
	BlockArray::BlockArray(uint16_t block_amount)
		: m_block_amount{block_amount}
	{
	}
	block_id BlockArray::get(glm::ivec3 block_pos) const
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

	void BlockArray::fill(block_id block_type)
	{
		memset(block_array, block_type, sizeof(block_array));
	}

	int BlockArray::getBlockIndex(glm::ivec3 block_pos) const
	{
		return block_pos.x * m_block_amount * m_block_amount + block_pos.y * m_block_amount + block_pos.z;
	}

	bool BlockArray::outOfBounds(glm::ivec3 block_pos) const
	{
		return outOfBounds(block_pos.x, m_block_amount)
			|| outOfBounds(block_pos.y, m_block_amount)
			|| outOfBounds(block_pos.z, m_block_amount);
	}

	bool BlockArray::outOfBounds(int coord, int block_amount) const
	{
		return coord < 0 || coord > block_amount;
	}

}
