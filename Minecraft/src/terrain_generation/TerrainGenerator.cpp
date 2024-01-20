#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(int world_seed, uint8_t surface_height, uint8_t water_height)
	: m_world_seed{world_seed},
	m_shape_generator{world_seed},
	m_min_surface_height{surface_height},
	m_water_height{water_height}
{
}

TerrainGenerator::TerrainGenerator()
	: m_world_seed{ 1337 },
	m_shape_generator{ 1337 },
	m_min_surface_height{ 20 },
	m_water_height{ 10 }
{

}
void TerrainGenerator::generateChunkTerrain(Chunk& chunk)
{
	if (chunk.isTerrainGenerated())
		return;

	glm::ivec2 chunk_pos_xz = chunk.getPosXZ();
	NoiseMap surface_map{ m_shape_generator.generateSurfaceMap(chunk)};
	
	LayerGenerator layer_generator(m_world_seed, m_min_surface_height, m_water_height);
	layer_generator.processChunk(chunk, surface_map);

	chunk.setIsTerrainGenerated(true);
}

ShapeGenerator& TerrainGenerator::getShapeGenerator()
{
	return m_shape_generator;
}
