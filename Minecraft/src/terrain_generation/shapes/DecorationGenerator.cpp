#include "DecorationGenerator.h"

DecorationGenerator::DecorationGenerator(TreeShape tree_shape, TreeHeightBoundaries tree_height_boundaries, int seed)
	: m_tree_shape{ tree_shape },
	m_tree_height_boundaries{ tree_height_boundaries },
	m_seed{ seed }
{
}

void DecorationGenerator::decorateChunkTerrain(Chunk& chunk, NoiseMap& surface_map)
{
	glm::ivec3 chunk_pos = chunk.getPos();
	NoiseGenerator decoration_generator;
	NoiseMap tree_map = decoration_generator.generateHeightMap(chunk_pos, NoiseSettings::TreeSettings, m_seed);

	int chunk_world_y = chunk_pos.y * CHUNK_SIZE_Y;

	//TODO: refactor
	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int z = 0; z < CHUNK_SIZE_X; z++)
		{
			float tree_on = (tree_map[x][z] + 1.0f);
			tree_on *= 1000;
			bool place_tree = (int)tree_on % 125 == 0;
			if (place_tree && surface_map[x][z] < m_tree_height_boundaries.height_max)
			{
				if (isOnSurfaceChunk(chunk_world_y, surface_map[x][z]))
				{
					Tree tree{ m_tree_shape.trunk_height, m_tree_shape.crown_height, m_tree_shape.crown_height };
					uint8_t tree_plant_height = static_cast<uint8_t>(surface_map[x][z]) % CHUNK_SIZE_Y;
					if (chunk.getBlockId({ x, tree_plant_height, z }) == Block::GRASS)
						tree.addTree(chunk, { x, tree_plant_height, z });
				}
			}
		}
	}
}

bool DecorationGenerator::isOnSurfaceChunk(int chunk_pos_y, int height)
{
	return height >= chunk_pos_y && height <= chunk_pos_y + CHUNK_SIZE_Y;
}
