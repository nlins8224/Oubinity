#include "BiomeGenerator.h"

void BiomeGenerator::processChunk(Chunk& chunk, const height_map& height_map)
{
	glm::ivec3 chunk_pos{ chunk.getPosition() };

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = 0; z < CHUNK_SIZE_Z; z++)
			{
				glm::ivec3 block_pos{ x, y, z };
				glm::ivec3 block_world_pos = static_cast<glm::ivec3>(chunk.getWorldPos()) + block_pos;

				if (block_world_pos.y == height_map[x][z])
					chunk.setBlock(block_pos, Block::GRASS);
				else if (block_world_pos.y < height_map[x][z] && block_world_pos.y > height_map[x][z] - 10)
					chunk.setBlock(block_pos, Block::DIRT);
				else if (block_world_pos.y < height_map[x][z])
					chunk.setBlock(block_pos, Block::STONE);
				else
					chunk.setBlock(block_pos, Block::AIR);
			}
		}
	}
}