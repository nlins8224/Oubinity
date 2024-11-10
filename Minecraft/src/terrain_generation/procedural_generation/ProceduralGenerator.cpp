#include "ProceduralGenerator.h"

ProceduralGenerator::ProceduralGenerator(int world_seed, uint8_t water_height)
	: m_world_seed{ world_seed },
	m_shape_generator{ world_seed },
	m_water_height{ water_height }
{
}

ProceduralGenerator::ProceduralGenerator()
	: m_world_seed{ 1337 },
	m_shape_generator{ 1337 },
	m_water_height{ 10 }
{

}

HeightMap ProceduralGenerator::generateHeightMap(Chunk& chunk)
{
	HeightMap height_map = m_shape_generator.generateSurfaceMap(chunk);
	return height_map;
}

HeightMap ProceduralGenerator::generateHeightMap(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod)
{
	HeightMap height_map = m_shape_generator.generateSurfaceMap(chunk_pos, lod);
	return height_map;
}

bool ProceduralGenerator::generateLayers(Chunk& chunk, HeightMap height_map)
{
	LayerGenerator layer_generator(m_world_seed, m_water_height);
	return layer_generator.processChunk(chunk, height_map);
}

void ProceduralGenerator::generateTrees(Chunk& chunk)
{
	glm::ivec3 chunk_pos = chunk.getPos();
	LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
	TreePresenceMap tree_presence_map = m_shape_generator.generateTreePresenceMap(chunk_pos, lod, NoiseSettings::TreeSettings, m_world_seed);
	HeightMap height_map = generateHeightMap(chunk);

	m_decoration_generator.generateTrees(chunk, height_map, tree_presence_map, m_water_height);
}

void ProceduralGenerator::generateTrees(Chunk& chunk, HeightMap& height_map)
{
	glm::ivec3 chunk_pos = chunk.getPos();
	LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
	TreePresenceMap tree_presence_map = m_shape_generator.generateTreePresenceMap(chunk_pos, lod, NoiseSettings::TreeSettings, m_world_seed);
	m_decoration_generator.generateTrees(chunk, height_map, tree_presence_map, m_water_height);
}

uint8_t ProceduralGenerator::getWaterHeight()
{
	return m_water_height;
}