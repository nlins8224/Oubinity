#pragma once
#include <array>
#include <glm/glm.hpp>
#include "Block.h"
#include "../chunk/ChunkSize.h"


namespace Block
{
class BlockArray
{
public:
	BlockArray();
	virtual ~BlockArray() = default;

	block_id get(glm::ivec3 block_pos) const;
	void set(glm::ivec3 block_pos, block_id block_type);
private:
	int getBlockIndex(glm::ivec3 block_pos) const;
	std::array<block_id, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> block_array {block_id::AIR};
};

}
