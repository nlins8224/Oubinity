#pragma once
#include "../chunk/Chunk.h"

namespace Block
{
class BlockArray
{
public:
	block_id get(glm::ivec3 block_pos);
	void set(glm::ivec3 block_pos, block_id block_type);
private:
	int getBlockIndex(glm::ivec3 block_pos);
	bool outOfBounds(glm::ivec3 block_pos);
	bool outOfBounds(int coord, int chunk_size);
	block_id block_array[Chunk::CHUNK_SIZE_X * Chunk::CHUNK_SIZE_Y * Chunk::CHUNK_SIZE_Z];
};

}
