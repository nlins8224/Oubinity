#include "BiomeGenerator.h"

BiomeGenerator::BiomeGenerator(int seed, uint8_t surface_height, uint8_t water_height)
	: m_seed{seed},
	m_min_surface_height{surface_height},
	m_water_height{water_height}
{
}

void BiomeGenerator::processChunk(Chunk& chunk, const HeightMap& height_map)
{
	// This is for optimization purposes; processing each block separately is slow
	if (isBelowSurface(chunk.getWorldPos().y))
	{
		chunk.getBlockArray().fill(Block::STONE);
		return;
	}

	auto layer_handler = std::make_shared<OceanLayerHandler>(m_water_height);
	auto surface_layer = std::make_shared<SurfaceLayerHandler>();
	auto underground_layer = std::make_shared<UndergroundLayerHandler>();

	layer_handler
		->addNextLayer(surface_layer)
		->addNextLayer(underground_layer);

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = 0; z < CHUNK_SIZE_Z; z++)
			{
				layer_handler->handle(chunk, { x, y, z }, height_map[x][z], m_seed);
			}
		}
	}
}

bool BiomeGenerator::isBelowSurface(uint8_t height)
{
	return height < m_min_surface_height;
}