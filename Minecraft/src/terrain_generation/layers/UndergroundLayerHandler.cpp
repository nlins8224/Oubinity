#include "UndergroundLayerHandler.h"

bool UndergroundLayerHandler::tryHandle(Chunk& chunk, glm::ivec3 block_pos, int surface_height, int seed)
{
	int block_size = chunk.getLevelOfDetail().block_size;
	glm::ivec3 block_world_pos = static_cast<glm::ivec3>(chunk.getWorldPos()) + (block_pos * block_size);
	if (block_world_pos.y < surface_height)
	{
		chunk.setBlock(block_pos, Block::STONE);
		return true;
	}
	return false;
}
