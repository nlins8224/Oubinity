#include "LayerGenerator.h"

LayerGenerator::LayerGenerator(int seed, uint8_t water_height)
	: m_seed{seed},
	m_water_height{water_height}
{
}

bool LayerGenerator::processChunk(Chunk& chunk, const HeightMap& height_map)
{		
	int block_amount = chunk.getLevelOfDetail().block_amount;
	int block_size = chunk.getLevelOfDetail().block_size;
	glm::ivec3 chunk_world_pos = chunk.getPos() * CHUNK_SIZE;

	bool anything_added = false;
	for (int x = 0; x < block_amount; x++)
	{
		for (int y = 0; y < block_amount; y++)
		{
			for (int z = 0; z < block_amount; z++)
			{
				glm::ivec3 block_pos = { x, y, z };
				int surface_height = height_map[x][z];
				glm::ivec3 block_world_pos = chunk_world_pos + (block_pos * block_size);
				if (surface_height > block_world_pos.y - block_size && surface_height < block_world_pos.y + block_size)
				{
					chunk.setBlock(block_pos, Block::GRASS);
					anything_added = true;
				}
				else {
					chunk.setBlock(block_pos, Block::AIR);
				}
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
	return anything_added;
}

bool LayerGenerator::generatePreloadedChunkUndergroundLayer(Chunk& chunk, const HeightMap& height_map)
{
	int block_amount = chunk.getLevelOfDetail().block_amount;
	int block_size = chunk.getLevelOfDetail().block_size;
	bool anything_added = false;
	for (int x = 0; x < block_amount; x++)
	{
		for (int z = 0; z < block_amount; z++)
		{
			int l_y = ((int)height_map[x][z] % block_amount) - 1;
			std::pair<bool, glm::ivec3> should_add_blocks = shouldAddBlocksToColumn(chunk, x, l_y, z );
			if (!should_add_blocks.first) {
				continue;
			}

			int end_pos = should_add_blocks.second.y;
			for (int y = l_y; y > end_pos; y--)
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

bool LayerGenerator::isBlockInSurfaceHeightBounds(glm::ivec3 block_pos, glm::ivec3 chunk_pos, int surface_height, int block_size)
{
	glm::ivec3 block_world_pos = chunk_pos * CHUNK_SIZE + (block_pos * block_size);
	int max_surface_height = std::min(block_world_pos.y + block_size, 255);
	int min_surface_height = std::max(block_world_pos.y - (block_size * Settings::SETTING_BLOCK_MARGIN), 0);
	return surface_height >= min_surface_height && surface_height <= max_surface_height;
}

bool LayerGenerator::hasBlocksInHorizontalNeighborhood(Chunk& chunk, glm::ivec3 block_pos)
{
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;
	glm::ivec3 left{ x - 1, y, z }, right{ x + 1, y, z }, forward{ x, y, z - 1 }, backward{ x, y, z + 1 };
	return chunk.isBlockPresent(left) || chunk.isBlockPresent(right) || chunk.isBlockPresent(forward) || chunk.isBlockPresent(backward);
}

std::pair<bool, glm::ivec3> LayerGenerator::shouldAddBlocksToColumn(Chunk& chunk, int col_x, int heightmap_y, int col_z)
{
	bool found_anything = false;
	glm::ivec3 found_at_pos{ 0, 0, 0 };
	for (int y = heightmap_y - 12; y <= heightmap_y; y++) {
		if (hasBlocksInHorizontalNeighborhood(chunk, {col_x, y, col_z})) {
			found_anything = true;
			found_at_pos = { col_x, y, col_z };
			return { found_anything, found_at_pos };
		}
	}
	return { found_anything, found_at_pos };
}
