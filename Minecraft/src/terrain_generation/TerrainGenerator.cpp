#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(int world_seed)
	: m_world_seed{world_seed},
	m_shape_generator{world_seed}
{
}

void TerrainGenerator::generateChunkTerrain(Chunk& chunk)
{
	if (chunk.isTerrainGenerated())
		return;


	m_shape_generator.generateSurfaceMap(chunk);
	glm::ivec2 chunk_pos_xz = chunk.getPosXZ();
	HeightMap surface_map{ m_shape_generator.getSurfaceMap({chunk_pos_xz}) };
	
	BiomeGenerator biome_generator(m_world_seed);
	biome_generator.processChunk(chunk, surface_map);

	//for (int x = 0; x < CHUNK_SIZE_X; x++)
	//{
	//	for (int y = 0; y < CHUNK_SIZE_Y; y++)
	//	{
	//		for (int z = 0; z < CHUNK_SIZE_Z; z++)
	//		{
	//			glm::ivec3 block_pos{ x, y, z };
	//			glm::ivec3 block_world_pos = static_cast<glm::ivec3>(chunk.getWorldPos()) + block_pos;

	//			if (block_world_pos.y > surface_map[x][z])
	//			{
	//				chunk.setBlock(block_pos, Block::AIR);
	//			}
	//			else
	//			{
	//				chunk.setBlock(block_pos, Block::STONE);
	//			}
	//		}
	//	}
	//}

	chunk.setIsTerrainGenerated(true);
}

ShapeGenerator& TerrainGenerator::getShapeGenerator()
{
	return m_shape_generator;
}

float TerrainGenerator::getSurfaceHeight(glm::ivec2 chunk_pos_xz, glm::ivec2 block_pos_xz)
{
	return m_shape_generator.getSurfaceHeight(chunk_pos_xz, block_pos_xz);
}

