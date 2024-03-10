#include "ProceduralGenerator.h"

ProceduralGenerator::ProceduralGenerator(int world_seed, uint8_t surface_height, uint8_t water_height)
	: m_world_seed{ world_seed },
	m_shape_generator{ world_seed },
	m_min_surface_height{ surface_height },
	m_water_height{ water_height }
{
}

ProceduralGenerator::ProceduralGenerator()
	: m_world_seed{ 1337 },
	m_shape_generator{ 1337 },
	m_min_surface_height{ 20 },
	m_water_height{ 10 }
{

}

HeightMap ProceduralGenerator::generateHeightMap(Chunk& chunk)
{
	HeightMap height_map = m_shape_generator.generateSurfaceMap(chunk);
	m_heightmaps.emplace(chunk.getPos(), height_map);
	return height_map;
}

void ProceduralGenerator::generateLayers(Chunk& chunk, HeightMap height_map)
{
	LayerGenerator layer_generator(m_world_seed, m_min_surface_height, m_water_height);
	layer_generator.processChunk(chunk, height_map);
}

void ProceduralGenerator::generateLayers(Chunk& chunk, HeightMap height_map, BlockMap block_map)
{
	LayerGenerator layer_generator(m_world_seed, m_min_surface_height, m_water_height);
	layer_generator.processChunk(chunk, height_map, block_map);
}


void ProceduralGenerator::generateTrees(Chunk& chunk)
{
	glm::ivec3 chunk_pos = chunk.getPos();
	LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
	TreePresenceMap tree_presence_map = m_shape_generator.generateTreePresenceMap(chunk_pos, lod, NoiseSettings::TreeSettings, m_world_seed);
	m_decoration_generator.generateTrees(chunk, m_heightmaps.at(chunk_pos), tree_presence_map);
}

uint8_t ProceduralGenerator::getWaterHeight()
{
	return m_water_height;
}
