#include "LayerGenerator.h"

LayerGenerator::LayerGenerator(int seed, uint8_t water_height)
	: m_seed{seed},
	m_water_height{water_height}
{
}

bool LayerGenerator::processChunk(Chunk& chunk, const HeightMap& height_map)
{		
	int block_amount = chunk.getLevelOfDetail().block_amount;
	int block_size = chunk.getLevelOfDetail().block_size;
	glm::ivec3 chunk_world_pos = chunk.getPos() * CHUNK_SIZE;

	bool anything_added = false;
	for (int x = 0; x < block_amount; x++)
	{
		for (int y = 0; y < block_amount; y++)
		{
			for (int z = 0; z < block_amount; z++)
			{
				glm::ivec3 block_pos = { x, y, z };
				int surface_height = height_map[x][z];
				glm::ivec3 block_world_pos = chunk_world_pos + (block_pos * block_size);
				if (surface_height > block_world_pos.y - block_size && surface_height < block_world_pos.y + block_size)
				{
					chunk.setBlock(block_pos, Block::GRASS);
					anything_added = true;
				}
				else {
					chunk.setBlock(block_pos, Block::AIR);
				}
			}
		}
	}
	return anything_added;
}

