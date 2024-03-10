#include "DecorationGenerator.h"

DecorationGenerator::DecorationGenerator()
{
	m_tree_shape.trunk_height = 6;
	m_tree_shape.crown_height = 7;
	m_tree_shape.crown_width = 7;
	m_tree_height_boundaries.height_min = 0;
	m_tree_height_boundaries.height_max = 9;
}

void DecorationGenerator::generateTrees(Chunk& chunk, HeightMap& surface_map, TreePresenceMap tree_presence_map, uint8_t water_height)
{
	glm::ivec3 chunk_pos = chunk.getPos();
	LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
	int chunk_world_y = chunk_pos.y * CHUNK_SIZE;

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			if (tree_presence_map[x][z] && isChunkPosOnSurface(chunk_world_y, surface_map[x][z]))
			{
				Tree tree{ m_tree_shape.trunk_height, m_tree_shape.crown_height, m_tree_shape.crown_height };
				uint8_t tree_plant_height = static_cast<uint8_t>(surface_map[x][z]) % CHUNK_SIZE;
				if (chunk.getBlockId({ x, tree_plant_height, z }) == Block::GRASS && tree_plant_height > water_height) {
					tree.addTree(chunk, { x, tree_plant_height, z });
				}
			}

		}
	}
}

bool DecorationGenerator::isChunkPosOnSurface(int chunk_pos_y, int height)
{
	return height >= chunk_pos_y && height <= chunk_pos_y + CHUNK_SIZE;
}
