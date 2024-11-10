#include "PreloadedGenerator.h"

PreloadedGenerator::PreloadedGenerator(uint8_t water_height, glm::vec3 scale)
	:
	m_water_height{ water_height },
	m_height_maps{ PreloadedGeneration::parsePNGToHeightMaps_8BIT("assets/gaea30.png", scale)},
	m_block_maps{ PreloadedGeneration::parsePNGToBlockMaps("assets/gaea30_colormap.png", scale)},
	m_tree_maps{ PreloadedGeneration::parsePNGToHeightMaps_8BIT("assets/gaea31_treemap.png", scale)}
{
}

HeightMap& PreloadedGenerator::getHeightMap(glm::ivec3 chunk_pos)
{
	glm::ivec3 chunk_pos_in_heightmap = mapChunkPosToHeightMapPos(chunk_pos);
	return m_height_maps.at(chunk_pos_in_heightmap.x * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS + chunk_pos_in_heightmap.z);
}

BlockMap& PreloadedGenerator::getBlockMap(glm::ivec3 chunk_pos)
{
	glm::ivec3 chunk_pos_in_map = mapChunkPosToHeightMapPos(chunk_pos);
	return m_block_maps.at(chunk_pos_in_map.x * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS + chunk_pos_in_map.z);
}

HeightMap& PreloadedGenerator::getTreeMap(glm::ivec3 chunk_pos)
{
	glm::ivec3 chunk_pos_in_map = mapChunkPosToHeightMapPos(chunk_pos);
	return m_tree_maps.at(chunk_pos_in_map.x * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS + chunk_pos_in_map.z);

}

TreePresenceMap PreloadedGenerator::generateTreePresenceMap(HeightMap& tree_map)
{
	TreePresenceMap tree_presence_map{};
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			tree_presence_map[x][z] = tree_map[x][z] > 0.5;
		}
	}
	return tree_presence_map;
}

glm::ivec3 PreloadedGenerator::mapChunkPosToHeightMapPos(glm::ivec3 chunk_pos)
{
	return chunk_pos + ((ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS) / 2);
}


void PreloadedGenerator::generateTrees(Chunk& chunk)
{
	glm::ivec3 chunk_pos = chunk.getPos();
	HeightMap& height_map = getHeightMap(chunk_pos);
	TreePresenceMap tree_presence_map = generateTreePresenceMap(getTreeMap(chunk_pos));
	m_decoration_generator.generateTrees(chunk, height_map, tree_presence_map, m_water_height);
}
