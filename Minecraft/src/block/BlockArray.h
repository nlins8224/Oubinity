#pragma once
#include <array>
#include <glm/glm.hpp>
#include "Block.h"
#include "../level_of_detail/LevelOfDetail.h"

namespace Block
{
class BlockArray
{
public:
	BlockArray(LevelOfDetail::LevelOfDetail lod);
	virtual ~BlockArray();

	block_id get(glm::ivec3 block_pos) const;
	void set(glm::ivec3 block_pos, block_id block_type);
	void fill(block_id block_type);
private:
	int getBlockIndex(glm::ivec3 block_pos) const;
	block_id* block_array;
	LevelOfDetail::LevelOfDetail m_lod;
};

}
