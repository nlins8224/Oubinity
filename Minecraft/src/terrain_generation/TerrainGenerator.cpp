#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(int world_seed, const int render_distance)
	: m_world_seed{world_seed}
{

}

void TerrainGenerator::generateChunkTerrain(Chunk& chunk, const int render_distance)
{
	if (chunk.isTerrainGenerated())
		return;

	ShapeGenerator shape_generator(m_world_seed);

	HeightMap surface_map{ shape_generator.generateSurfaceMap(chunk)};
	BiomeGenerator biome_generator(m_world_seed);
	//biome_generator.processChunk(chunk, surface_map);


	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = 0; z < CHUNK_SIZE_Z; z++)
			{
				glm::ivec3 block_pos{ x, y, z };
				glm::ivec3 block_world_pos = static_cast<glm::ivec3>(chunk.getWorldPos()) + block_pos;

				if (block_world_pos.y > surface_map[x][z])
				{
					chunk.setBlock(block_pos, Block::AIR);
				}
				else
				{
					chunk.setBlock(block_pos, Block::STONE);
				}
			}
		}
	}

	chunk.setIsTerrainGenerated(true);
}

