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

bool ProceduralGenerator::generateLayers(Chunk& chunk, HeightMap height_map)
{
	LayerGenerator layer_generator(m_world_seed, m_water_height);
	return layer_generator.processChunk(chunk, height_map);
}

bool ProceduralGenerator::generateLayers(Chunk& chunk, HeightMap height_map, BlockMap block_map)
{
	LayerGenerator layer_generator(m_world_seed, m_water_height);
	return layer_generator.processChunk(chunk, height_map, block_map);
}


void ProceduralGenerator::generateTrees(Chunk& chunk)
{
	glm::ivec3 chunk_pos = chunk.getPos();
	LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
	TreePresenceMap tree_presence_map = m_shape_generator.generateTreePresenceMap(chunk_pos, lod, NoiseSettings::TreeSettings, m_world_seed);
	m_decoration_generator.generateTrees(chunk, m_heightmaps.at(chunk_pos), tree_presence_map, m_water_height);
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

bool ProceduralGenerator::isChunkBelowOrAboveSurface(Chunk& chunk, const HeightMap& height_map)
{
	int block_amount = chunk.getLevelOfDetail().block_amount;
	double min_height = 99999;
	double max_height = -99999;
	for (int x = 0; x < block_amount; x++)
	{
		for (int z = 0; z < block_amount; z++)
		{
			min_height = std::min(min_height, height_map[x][z]);
			max_height = std::max(max_height, height_map[x][z]);
		}
	}
	// Real CHUNK_SIZE here is correct
	int chunk_pos_y = chunk.getPos().y * CHUNK_SIZE;
	bool below_surface = chunk_pos_y + CHUNK_SIZE < min_height;
	bool above_surface = chunk_pos_y > max_height;
	return below_surface || above_surface;
}