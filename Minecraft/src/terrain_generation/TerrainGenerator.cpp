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
	m_min_surface_height{ 60 },
	m_water_height{ 40 }
{

}
void TerrainGenerator::generateChunkTerrain(Chunk& chunk)
{
	if (chunk.isTerrainGenerated())
		return;

	m_shape_generator.generateSurfaceMap(chunk);
	glm::ivec2 chunk_pos_xz = chunk.getPosXZ();
	NoiseMap surface_map{ m_shape_generator.getSurfaceMap({chunk_pos_xz}) };
	
	BiomeGenerator biome_generator(m_world_seed, m_min_surface_height, m_water_height);
	biome_generator.processChunk(chunk, surface_map);

	chunk.setIsTerrainGenerated(true);
}

//void TerrainGenerator::decorateChunkTerrain(Chunk& chunk)
//{
//	if (!chunk.isTerrainGenerated())
//		return;
//
//	TreeShape tree_shape{ 7, 5, 5 };
//	TreeHeightBoundaries tree_height_boundaries{ 0, 120 };
//	DecorationGenerator decoration_generator{tree_shape, tree_height_boundaries, m_world_seed};
//
//	glm::ivec2 chunk_pos_xz = chunk.getPosXZ();
//	decoration_generator.decorateChunkTerrain(chunk, m_shape_generator.getSurfaceMap({ chunk_pos_xz }));
//}

ShapeGenerator& TerrainGenerator::getShapeGenerator()
{
	return m_shape_generator;
}

float TerrainGenerator::getSurfaceHeight(glm::ivec2 chunk_pos_xz, glm::ivec2 block_pos_xz)
{
	return m_shape_generator.getSurfaceHeight(chunk_pos_xz, block_pos_xz);
}