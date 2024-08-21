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

	layer_handler
		->addNextLayer(underground_layer);
		
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
	auto layer_handler = std::make_shared<SurfacePreloadedLayerHandler>(block_map);
	auto underground_layer = std::make_shared<UndergroundLayerHandler>();

	layer_handler
		->addNextLayer(underground_layer);

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

