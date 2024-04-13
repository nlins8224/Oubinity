#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(int world_seed, uint8_t water_height)
	:m_water_height{water_height}
{
#if SETTING_USE_PRELOADED_HEIGHTMAP || SETTING_USE_PRELOADED_LAYERS
	m_preloaded_generator = PreloadedGenerator();
#endif

	m_procedural_generator = ProceduralGenerator(world_seed, water_height);
}

TerrainGenerator::TerrainGenerator()
{
#if SETTING_USE_PRELOADED_HEIGHTMAP || SETTING_USE_PRELOADED_COLORMAP
	m_preloaded_generator = PreloadedGenerator();
#endif
	m_procedural_generator = ProceduralGenerator();

}
bool TerrainGenerator::generateChunkTerrain(Chunk& chunk)
{
	if (chunk.isTerrainGenerated())
		return false;

	HeightMap height_map = generateHeightMap(chunk);
	bool is_chunk_visible = !m_procedural_generator.isChunkBelowOrAboveSurface(chunk, height_map);
	generateChunkTerrain(chunk, height_map, is_chunk_visible);
	chunk.setIsTerrainGenerated(true);
	return is_chunk_visible;
}

void TerrainGenerator::generateChunkTerrain(Chunk& chunk, HeightMap& height_map, bool is_chunk_visible)
{
	if (is_chunk_visible)
	{
		chunk.setBlockArray();
		generateLayers(chunk, height_map);
	}
	chunk.setIsVisible(is_chunk_visible);
}

uint8_t TerrainGenerator::getWaterHeight()
{
	return m_water_height;
}

HeightMap TerrainGenerator::generateHeightMap(Chunk& chunk)
{
	glm::ivec3 chunk_pos = chunk.getPos();
	LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
#if SETTING_USE_PRELOADED_HEIGHTMAP
	return m_preloaded_generator.generateHeightMap(chunk_pos, lod);
#else 
	return m_procedural_generator.generateHeightMap(chunk_pos, lod);
#endif
}

HeightMap TerrainGenerator::generateHeightMap(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod)
{
#if SETTING_USE_PRELOADED_HEIGHTMAP
	return m_preloaded_generator.generateHeightMap(chunk_pos, lod);
#else 
	return m_procedural_generator.generateHeightMap(chunk_pos, lod);
#endif
}


bool TerrainGenerator::generateLayers(Chunk& chunk, HeightMap height_map)
{
#if SETTING_USE_PRELOADED_COLORMAP
	BlockMap block_map = m_preloaded_generator.generateBlockMap(chunk);
	return m_procedural_generator.generateLayers(chunk, height_map, block_map);
#else 
	return m_procedural_generator.generateLayers(chunk, height_map);
#endif
}

void TerrainGenerator::generateTrees(Chunk& chunk)
{
#if SETTING_USE_PRELOADED_HEIGHTMAP
	HeightMap height_map = m_preloaded_generator.generateHeightMap(chunk);
	m_procedural_generator.generateTrees(chunk, height_map);
#else
	m_procedural_generator.generateTrees(chunk);
#endif	
}

bool TerrainGenerator::isChunkBelowOrAboveSurface(glm::ivec3 chunk_pos, HeightMap& height_map, LevelOfDetail::LevelOfDetail lod)
{
	return m_procedural_generator.isChunkBelowOrAboveSurface(chunk_pos, height_map, lod);
}