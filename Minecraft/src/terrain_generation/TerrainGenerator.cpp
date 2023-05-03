#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(int world_seed, uint8_t surface_height, uint8_t water_height)
	: m_world_seed{world_seed},
	m_shape_generator{world_seed},
	m_min_surface_height{surface_height},
	m_water_height{water_height}
{
}

void TerrainGenerator::generateChunkTerrain(Chunk& chunk)
{
	OPTICK_EVENT();
	if (chunk.isTerrainGenerated())
		return;


	m_shape_generator.generateSurfaceMap(chunk);
	glm::ivec2 chunk_pos_xz = chunk.getPosXZ();
	HeightMap surface_map{ m_shape_generator.getSurfaceMap({chunk_pos_xz}) };
	
	BiomeGenerator biome_generator(m_world_seed, m_min_surface_height, m_water_height);
	biome_generator.processChunk(chunk, surface_map);

	chunk.setIsTerrainGenerated(true);
}

void TerrainGenerator::decorateChunkTerrain(Chunk& chunk)
{
	if (!chunk.isTerrainGenerated())
		return;

	glm::ivec3 chunk_pos = chunk.getPos();
	HeightMap surface_map{ m_shape_generator.getSurfaceMap({chunk_pos.x, chunk_pos.z}) };
	int chunk_world_y = chunk_pos.y * CHUNK_SIZE_Y;

	for (int i = 0; i < CHUNK_SIZE_X; i++)
	{
		for (int j = 0; j < CHUNK_SIZE_X; j++)
		{
			if (i % 10 == 0 && j % 10 == 0)
			{
				if (isOnSurfaceChunk(chunk_world_y, surface_map[i][j]))
				{
					Tree tree{ 8, 5, 7 };
					uint8_t tree_plant_height = static_cast<uint8_t>(surface_map[i][j]) % CHUNK_SIZE_Y;
					if (chunk.getBlockId({ i, tree_plant_height, j }) == Block::GRASS)
						tree.addTree(chunk, { i, tree_plant_height, j });
				}
			}
		}
	}		
}

ShapeGenerator& TerrainGenerator::getShapeGenerator()
{
	return m_shape_generator;
}

float TerrainGenerator::getSurfaceHeight(glm::ivec2 chunk_pos_xz, glm::ivec2 block_pos_xz)
{
	return m_shape_generator.getSurfaceHeight(chunk_pos_xz, block_pos_xz);
}

bool TerrainGenerator::isOnSurfaceChunk(int chunk_pos_y, int height)
{
	return height >= chunk_pos_y && height <= chunk_pos_y + CHUNK_SIZE_Y;
}