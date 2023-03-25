#include "AirLayerHandler.h"

bool AirLayerHandler::tryHandle(Chunk& chunk, glm::ivec3 block_pos, int surface_height, int seed)
{
	glm::ivec3 block_world_pos = static_cast<glm::ivec3>(chunk.getWorldPos()) + block_pos;

	if (block_world_pos.y > surface_height)
	{
		chunk.setBlock(block_pos, Block::AIR);
		return true;
	}

	return false;
}
