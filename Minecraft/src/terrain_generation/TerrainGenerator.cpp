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
	bool is_chunk_visible = !isChunkBelowOrAboveSurface(chunk, height_map);
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

#if SETTING_USE_PRELOADED_HEIGHTMAP || SETTING_USE_PRELOADED_COLORMAP

bool TerrainGenerator::generatePreloadedUndergroundLayer(Chunk& chunk, HeightMap& height_map)
{
	return m_preloaded_generator.generatePreloadedChunkUndergroundLayer(chunk, height_map);
}

PreloadedHeightMap TerrainGenerator::generatePreloadedHeightMap(glm::ivec3 chunk_pos)
{
	return m_preloaded_generator.getHeightMap(chunk_pos);
}

#endif

#if SETTING_USE_PRELOADED_COLORMAP
bool TerrainGenerator::generatePreloadedLayers(Chunk& chunk, HeightMap height_map)
{
	chunk.setBlockArray();
	BlockMap block_map = m_preloaded_generator.getBlockMap(chunk.getPos());
	return m_preloaded_generator.generateLayers(chunk, height_map, block_map);;
}
#endif

uint8_t TerrainGenerator::getWaterHeight()
{
	return m_water_height;
}

HeightMap TerrainGenerator::generateHeightMap(Chunk& chunk)
{
	return m_procedural_generator.generateHeightMap(chunk.getPos(), chunk.getLevelOfDetail());
}

HeightMap TerrainGenerator::generateHeightMap(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod)
{
	return m_procedural_generator.generateHeightMap(chunk_pos, lod);
}

bool TerrainGenerator::generateLayers(Chunk& chunk, HeightMap height_map)
{
	return m_procedural_generator.generateLayers(chunk, height_map);
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

bool TerrainGenerator::isChunkBelowOrAboveSurface(Chunk& chunk, const HeightMap& height_map)
{
	glm::ivec3 chunk_pos = chunk.getPos();
	LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
	return isChunkBelowOrAboveSurface(chunk_pos, height_map, lod);
}

bool TerrainGenerator::isChunkBelowOrAboveSurface(glm::ivec3 chunk_pos, const HeightMap& height_map, LevelOfDetail::LevelOfDetail lod)
{
	int block_amount = lod.block_amount;
	double min_height = std::numeric_limits<double>::max();
	double max_height = std::numeric_limits<double>::min();
	for (int x = 0; x < block_amount; x++)
	{
		for (int z = 0; z < block_amount; z++)
		{
			min_height = std::min(min_height, height_map[x][z]);
			max_height = std::max(max_height, height_map[x][z]);
		}
	}
	// Real CHUNK_SIZE here is correct
	int chunk_pos_y = chunk_pos.y * CHUNK_SIZE;
	bool below_surface = chunk_pos_y + CHUNK_SIZE < min_height;
	bool above_surface = chunk_pos_y > max_height;
	return below_surface || above_surface;
}