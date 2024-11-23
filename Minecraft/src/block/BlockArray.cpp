#include "BlockArray.h"
#include <cstring>
namespace Block
{
	BlockArray::BlockArray(LevelOfDetail::LevelOfDetail lod)
		: m_lod{lod}
	{
		int chunk_size_cubed = m_lod.block_amount * m_lod.block_amount * m_lod.block_amount;
		block_array = new block_id[chunk_size_cubed]();
	}
	BlockArray::~BlockArray()
	{
		delete block_array;
	}
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
		int chunk_size = m_lod.block_amount;
		return block_pos.x * chunk_size * chunk_size + block_pos.y * chunk_size + block_pos.z;
	}

	void BlockArray::fill(block_id block_type)
	{
		int chunk_size_cubed = m_lod.block_amount * m_lod.block_amount * m_lod.block_amount;
		memset(block_array, block_type, chunk_size_cubed);
	}
}
