#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(int world_seed, uint8_t surface_height, uint8_t water_height)
{
#if defined(SETTING_USE_PRELOADED_HEIGHTMAP) || defined(SETTING_USE_PRELOADED_LAYERS)
	m_procedural_generator = ProceduralGenerator(world_seed, surface_height, water_height);
#endif
}

TerrainGenerator::TerrainGenerator()
{
#if defined(SETTING_USE_PRELOADED_HEIGHTMAP) || defined(SETTING_USE_PRELOADED_LAYERS)
	m_procedural_generator = ProceduralGenerator();
#endif
}
void TerrainGenerator::generateChunkTerrain(Chunk& chunk)
{
	if (chunk.isTerrainGenerated())
		return;

	NoiseMap height_map = generateHeightMap(chunk);
	generateLayers(chunk, height_map);

	chunk.setIsTerrainGenerated(true);
}

NoiseMap TerrainGenerator::generateHeightMap(Chunk& chunk)
{
#if SETTING_USE_PRELOADED_HEIGHTMAP

#else 
	return m_procedural_generator.generateHeightMap(chunk);
#endif
}

void TerrainGenerator::generateLayers(Chunk& chunk, NoiseMap height_map)
{
#if SETTING_USE_PRELOADED_LAYERS
#else 
	m_procedural_generator.generateLayers(chunk, height_map);
#endif
}