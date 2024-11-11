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


bool PreloadedGenerator::generateLayers(Chunk& chunk, const HeightMap& height_map, const BlockMap& block_map)
{
	int block_amount = chunk.getLevelOfDetail().block_amount;
	int block_size = chunk.getLevelOfDetail().block_size;
	bool anything_added = false;

	for (int x = 0; x < block_amount; x++)
	{
		for (int z = 0; z < block_amount; z++)
		{
			int l_y = (int)height_map[x][z] % block_amount;
			glm::ivec3 block_pos{ x, l_y, z };
			if (isBlockInSurfaceHeightBounds(block_pos, chunk.getPos(), height_map[x][z], block_size)) {
				block_id block = block_map[block_pos.x][block_pos.z];
				chunk.setBlock(block_pos, block);
				anything_added = true;
			}
		}
	}
	return anything_added;
}

bool PreloadedGenerator::generatePreloadedChunkUndergroundLayer(Chunk& chunk, const HeightMap& height_map)
{
	int block_amount = chunk.getLevelOfDetail().block_amount;
	int block_size = chunk.getLevelOfDetail().block_size;
	bool anything_added = false;
	for (int x = 0; x < block_amount; x++)
	{
		for (int z = 0; z < block_amount; z++)
		{
			int l_y = ((int)height_map[x][z] % block_amount) - 1;
			for (int y = l_y; y > l_y - Settings::SETTING_PRELOADED_UNDEGROUND_LAYER_DEPTH; y--)
			{
				if (!isBlockInSurfaceHeightBounds({ x, l_y + 1, z }, chunk.getPos(), height_map[x][z], block_size)) {
					continue;
				}
				glm::ivec3 block_pos{ x, y, z };
				if (chunk.isBlockOutsideChunk(block_pos)) {
					Chunk* neighbor = chunk.findNeighborChunk(block_pos);
					if (neighbor) {
						glm::ivec3 neighbor_block_pos = chunk.findNeighborBlockPos(block_pos);
						neighbor->setBlock(neighbor_block_pos, block_id::STONE);
						anything_added = true;
					}
				}
				else {
					chunk.setBlock(block_pos, block_id::STONE);
					anything_added = true;
				}
			}
		}
	}
	return anything_added;
}

bool PreloadedGenerator::isBlockInSurfaceHeightBounds(glm::ivec3 block_pos, glm::ivec3 chunk_pos, int surface_height, int block_size)
{
	glm::ivec3 block_world_pos = chunk_pos * CHUNK_SIZE + (block_pos * block_size);
	int max_surface_height = std::min(block_world_pos.y + block_size, 255);
	int min_surface_height = std::max(block_world_pos.y - (block_size * Settings::SETTING_BLOCK_MARGIN), 0);
	return surface_height >= min_surface_height && surface_height <= max_surface_height;
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
