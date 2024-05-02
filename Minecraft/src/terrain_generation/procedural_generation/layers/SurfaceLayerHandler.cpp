#include "SurfaceLayerHandler.h"

bool SurfaceLayerHandler::tryHandle(Chunk& chunk, glm::ivec3 block_pos, int surface_height, int seed)
{
	int block_size = chunk.getLevelOfDetail().block_size;
	glm::ivec3 block_world_pos = chunk.getPos() * CHUNK_SIZE + (block_pos * block_size);
	if (surface_height > block_world_pos.y - block_size && surface_height < block_world_pos.y + block_size)
	{
		chunk.setBlock(block_pos, Block::STONE);
		return true;
	}

	return false;
}