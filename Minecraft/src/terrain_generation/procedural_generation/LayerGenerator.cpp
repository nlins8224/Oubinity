#include "LayerGenerator.h"

LayerGenerator::LayerGenerator(int seed, uint8_t water_height)
	: m_seed{seed},
	m_water_height{water_height}
{
}

bool LayerGenerator::processChunk(Chunk& chunk, const HeightMap& height_map)
{
	auto layer_handler = std::make_shared<SurfaceLayerHandler>();
	auto underground_layer = std::make_shared<UndergroundLayerHandler>();
	auto air_layer = std::make_shared<AirLayerHandler>();

	//layer_handler
	//	->addNextLayer(underground_layer);
		
	int block_amount = chunk.getLevelOfDetail().block_amount;
	bool anything_added = false;
	for (int x = 0; x < block_amount; x++)
	{
		for (int y = 0; y < block_amount; y++)
		{
			for (int z = 0; z < block_amount; z++)
			{
				anything_added |= layer_handler->handle(chunk, { x, y, z }, height_map[x][z], m_seed);
			}
		}
	}
	return anything_added;
}

bool LayerGenerator::processChunk(Chunk& chunk, const HeightMap& height_map, const BlockMap& block_map)
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

	for (int x = 0; x < block_amount; x++)
	{
		for (int z = 0; z < block_amount; z++)
		{
			int l_y = ((int)height_map[x][z] % block_amount) - 1;
			if (l_y < 0 || l_y >= block_amount) {
				continue;
			}
			for (int y = l_y; y >= l_y - 8 && y >= 0; y--)
			{
				glm::ivec3 block_pos{ x, y, z };
				if (!isBlockInSurfaceHeightBounds({x, l_y + 1, z}, chunk.getPos(), height_map[x][z], block_size)) {
					continue;
				}

				glm::ivec3 up{ x, y + 1, z };
				glm::ivec3 left{ x - 1, y, z };
				glm::ivec3 right{ x + 1, y, z };
				glm::ivec3 forward{ x, y, z - 1 };
				glm::ivec3 backward{ x, y, z + 1 };

				bool is_visible =
					!chunk.isBlockPresent(up)
					|| !chunk.isBlockPresent(left)
					|| !chunk.isBlockPresent(right)
					|| !chunk.isBlockPresent(forward)
					|| !chunk.isBlockPresent(backward);

				if (is_visible) {
					chunk.setBlock(block_pos, block_id::STONE);
					anything_added = true;
				}
			}
		}
	}
	return anything_added;
}

bool LayerGenerator::isBlockInSurfaceHeightBounds(glm::ivec3 block_pos, glm::ivec3 chunk_pos, int surface_height, int block_size)
{
	glm::ivec3 block_world_pos = chunk_pos * CHUNK_SIZE + (block_pos * block_size);
	int max_surface_height = std::min(block_world_pos.y + block_size, 255);
	int min_surface_height = std::max(block_world_pos.y - (block_size * Settings::SETTING_BLOCK_MARGIN), 0);
	return surface_height >= min_surface_height && surface_height <= max_surface_height;
}