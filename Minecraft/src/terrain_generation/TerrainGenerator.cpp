#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(int world_seed)
	: m_world_seed{world_seed},
	m_shape_generator{world_seed}
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
	
	BiomeGenerator biome_generator(m_world_seed);
	biome_generator.processChunk(chunk, surface_map);

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

