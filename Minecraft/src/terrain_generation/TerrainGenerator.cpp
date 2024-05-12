#include "TerrainGenerator.h"

#if SETTING_USE_PRELOADED_HEIGHTMAP || SETTING_USE_PRELOADED_COLORMAP
TerrainGenerator::TerrainGenerator(int world_seed, uint8_t water_height)
	: m_water_height{ water_height },
	m_procedural_generator{ ProceduralGenerator(world_seed, water_height) },
	m_preloaded_generator{ PreloadedGenerator(water_height) }
{
}
#else
TerrainGenerator::TerrainGenerator(int world_seed, uint8_t water_height)
	: m_water_height{ water_height },
	m_procedural_generator{ ProceduralGenerator(world_seed, water_height)}
{
}
#endif

bool TerrainGenerator::generateChunkTerrain(Chunk& chunk)
{
	HeightMap height_map = generateHeightMap(chunk);
	bool is_chunk_visible = !m_procedural_generator.isChunkBelowOrAboveSurface(chunk, height_map);
	generateChunkTerrain(chunk, height_map, is_chunk_visible);
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
#if SETTING_USE_PRELOADED_HEIGHTMAP
	return m_preloaded_generator.getHeightMap(chunk_pos);
#else 
	LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
	return m_procedural_generator.generateHeightMap(chunk_pos, lod);
#endif
}

HeightMap TerrainGenerator::generateHeightMap(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod)
{
#if SETTING_USE_PRELOADED_HEIGHTMAP
	return m_preloaded_generator.getHeightMap(chunk_pos);
#else 
	return m_procedural_generator.generateHeightMap(chunk_pos, lod);
#endif
}


bool TerrainGenerator::generateLayers(Chunk& chunk, HeightMap height_map)
{
#if SETTING_USE_PRELOADED_COLORMAP
	BlockMap block_map = m_preloaded_generator.getBlockMap(chunk.getPos());
	return m_procedural_generator.generateLayers(chunk, height_map, block_map);
#else 
	return m_procedural_generator.generateLayers(chunk, height_map);
#endif
}

void TerrainGenerator::generateTrees(Chunk& chunk)
{
#if SETTING_USE_PRELOADED_HEIGHTMAP
	#if SETTING_USE_PRELOADED_TREEMAP
		m_preloaded_generator.generateTrees(chunk);
	#else
		HeightMap height_map = m_preloaded_generator.getHeightMap(chunk.getPos());
		m_procedural_generator.generateTrees(chunk, height_map);
	#endif
#else
	m_procedural_generator.generateTrees(chunk);
#endif	
}

bool TerrainGenerator::isChunkBelowOrAboveSurface(glm::ivec3 chunk_pos, HeightMap& height_map, LevelOfDetail::LevelOfDetail lod)
{
	return m_procedural_generator.isChunkBelowOrAboveSurface(chunk_pos, height_map, lod);
}