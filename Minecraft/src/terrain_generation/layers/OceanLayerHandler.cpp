#include "OceanLayerHandler.h"

OceanLayerHandler::OceanLayerHandler(int water_threshold)
	: m_water_threshold{water_threshold}
{
}

bool OceanLayerHandler::tryHandle(Chunk& chunk, glm::ivec3 block_pos, int surface_height, int seed)
{
	int block_size = chunk.getLevelOfDetail().block_size;
	glm::ivec3 block_world_pos = static_cast<glm::ivec3>(chunk.getWorldPos()) + (block_pos * block_size);
	if (block_world_pos.y > surface_height && block_world_pos.y < m_water_threshold)
	{
		chunk.setBlock(block_pos, Block::WATER);
		return true;
	}
	if (block_world_pos.y == surface_height && block_world_pos.y <= m_water_threshold)
	{
		chunk.setBlock(block_pos, Block::SAND);
		return true;
	}

	return false;
}
