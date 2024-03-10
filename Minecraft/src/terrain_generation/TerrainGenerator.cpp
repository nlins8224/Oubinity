#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(int world_seed, uint8_t surface_height, uint8_t water_height)
	:m_water_height{water_height}
{
#if SETTING_USE_PRELOADED_HEIGHTMAP || SETTING_USE_PRELOADED_LAYERS
	m_preloaded_generator = PreloadedGenerator();
#endif

	m_procedural_generator = ProceduralGenerator(world_seed, surface_height, water_height);
}

TerrainGenerator::TerrainGenerator()
{
#if SETTING_USE_PRELOADED_HEIGHTMAP || SETTING_USE_PRELOADED_COLORMAP
	m_preloaded_generator = PreloadedGenerator();
#endif
	m_procedural_generator = ProceduralGenerator();

}
void TerrainGenerator::generateChunkTerrain(Chunk& chunk)
{
	if (chunk.isTerrainGenerated())
		return;

	HeightMap height_map = generateHeightMap(chunk);
	generateLayers(chunk, height_map);
	chunk.setIsTerrainGenerated(true);
}

uint8_t TerrainGenerator::getWaterHeight()
{
	return m_water_height;
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
#if SETTING_USE_PRELOADED_COLORMAP
	BlockMap block_map = m_preloaded_generator.generateBlockMap(chunk);
	m_procedural_generator.generateLayers(chunk, height_map, block_map);
#else 
	m_procedural_generator.generateLayers(chunk, height_map);
#endif
}

void TerrainGenerator::generateTrees(Chunk& chunk)
{
	m_procedural_generator.generateTrees(chunk);
}