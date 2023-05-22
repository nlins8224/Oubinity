#include "SurfaceLayerHandler.h"

bool SurfaceLayerHandler::tryHandle(Chunk& chunk, glm::ivec3 block_pos, int surface_height, int seed)
{
	int block_size = chunk.getLevelOfDetail().block_size;
	glm::ivec3 block_world_pos = static_cast<glm::ivec3>(chunk.getWorldPos()) + (block_pos * block_size);
	if (surface_height == block_world_pos.y)
	{
		chunk.setBlock(block_pos, Block::GRASS);
		return true;
	}

	return false;
}