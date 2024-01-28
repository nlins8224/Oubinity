#include "SurfacePreloadedLayerHandler.h"

SurfacePreloadedLayerHandler::SurfacePreloadedLayerHandler(BlockMap block_map)
	: m_block_map{block_map}
{
}

bool SurfacePreloadedLayerHandler::tryHandle(Chunk& chunk, glm::ivec3 block_pos, int surface_height, int seed)
{
	int block_size = chunk.getLevelOfDetail().block_size;
	glm::ivec3 block_world_pos = chunk.getPos() * CHUNK_SIZE + (block_pos * block_size);
	if (surface_height > block_world_pos.y - block_size && surface_height < block_world_pos.y + block_size)
	{
		block_id block = m_block_map[block_pos.x][block_pos.z];
		chunk.setBlock(block_pos, block);
		return true;
	}

	return false;
}