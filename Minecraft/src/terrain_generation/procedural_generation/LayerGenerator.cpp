#include "LayerGenerator.h"

LayerGenerator::LayerGenerator(int seed, uint8_t surface_height, uint8_t water_height)
	: m_seed{seed},
	m_min_surface_height{surface_height},
	m_water_height{water_height}
{
}

void LayerGenerator::processChunk(Chunk& chunk, const HeightMap& height_map)
{
	// This is for optimization purposes; processing each block separately is slow
	//if (isBelowSurface(chunk))
	//{
	//	chunk.getBlockArray().fill(Block::STONE);
	//	
	//	return;
	//}

	auto layer_handler = std::make_shared<OceanLayerHandler>(m_water_height);
	auto surface_layer = std::make_shared<SurfaceLayerHandler>();
	auto underground_layer = std::make_shared<UndergroundLayerHandler>();

	layer_handler
		->addNextLayer(underground_layer)
		->addNextLayer(surface_layer);
		

	int block_amount = chunk.getLevelOfDetail().block_amount;
	int block_size = chunk.getLevelOfDetail().block_size;
	for (int x = 0; x < block_amount; x++)
	{
		for (int y = 0; y < block_amount; y++)
		{
			for (int z = 0; z < block_amount; z++)
			{
				layer_handler->handle(chunk, { x, y, z }, height_map[x][z], m_seed);
			}
		}
	}
}

void LayerGenerator::processChunk(Chunk& chunk, const HeightMap& height_map, const BlockMap& block_map)
{
	// This is for optimization purposes; processing each block separately is slow
	//if (isBelowSurface(chunk))
	//{
	//	chunk.getBlockArray().fill(Block::STONE);
	//	return;
	//}

	auto layer_handler = std::make_shared<SurfacePreloadedLayerHandler>(block_map);
	auto underground_layer = std::make_shared<UndergroundLayerHandler>();

	layer_handler
		->addNextLayer(underground_layer);

	int block_amount = chunk.getLevelOfDetail().block_amount;
	for (int x = 0; x < block_amount; x++)
	{
		for (int y = 0; y < block_amount; y++)
		{
			for (int z = 0; z < block_amount; z++)
			{
				layer_handler->handle(chunk, { x, y, z }, height_map[x][z], m_seed);
			}
		}
	}
}

bool LayerGenerator::isBelowSurface(Chunk& chunk)
{
	int chunk_pos_y = chunk.getPos().y;
	// Real CHUNK_SIZE here is correct
	return chunk_pos_y < m_min_surface_height - CHUNK_SIZE;
}