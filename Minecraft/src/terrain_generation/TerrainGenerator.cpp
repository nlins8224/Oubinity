#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(int world_seed, uint8_t surface_height, uint8_t water_height)
{
#if SETTING_USE_PRELOADED_HEIGHTMAP || SETTING_USE_PRELOADED_LAYERS
	m_preloaded_generator = PreloadedGenerator();
#endif

#if SETTING_USE_PRELOADED_HEIGHTMAP == 0 || SETTING_USE_PRELOADED_LAYERS == 0
	m_procedural_generator = ProceduralGenerator(world_seed, surface_height, water_height);
#endif
}

TerrainGenerator::TerrainGenerator()
{
#if SETTING_USE_PRELOADED_HEIGHTMAP || SETTING_USE_PRELOADED_LAYERS
	m_preloaded_generator = PreloadedGenerator();
#endif

#if SETTING_USE_PRELOADED_HEIGHTMAP == 0 || SETTING_USE_PRELOADED_LAYERS == 0
	m_procedural_generator = ProceduralGenerator();
#endif
}
void TerrainGenerator::generateChunkTerrain(Chunk& chunk)
{
	if (chunk.isTerrainGenerated())
		return;

	HeightMap height_map = generateHeightMap(chunk);
	generateLayers(chunk, height_map);

	chunk.setIsTerrainGenerated(true);
}

HeightMap TerrainGenerator::generateHeightMap(Chunk& chunk)
{
#if SETTING_USE_PRELOADED_HEIGHTMAP
	return m_preloaded_generator.generateHeightMap(chunk);
#else 
	return m_procedural_generator.generateHeightMap(chunk);
#endif
}

void TerrainGenerator::generateLayers(Chunk& chunk, HeightMap height_map)
{
#if SETTING_USE_PRELOADED_LAYERS
#else 
	m_procedural_generator.generateLayers(chunk, height_map);
#endif
}