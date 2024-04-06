#include "LayerGenerator.h"

LayerGenerator::LayerGenerator(int seed, uint8_t water_height)
	: m_seed{seed},
	m_water_height{water_height}
{
}

void LayerGenerator::processChunk(Chunk& chunk, const HeightMap& height_map)
{
	chunk.getBlockArray().fill(Block::STONE);
	return;
	//if (isBelowSurface(chunk, height_map))
	//{
	//	chunk.setIsVisible(false);
	//	//chunk.getBlockArray().fill(Block::STONE);
	//	return;
	//}

	auto layer_handler = std::make_shared<SurfaceLayerHandler>();
	auto underground_layer = std::make_shared<UndergroundLayerHandler>();
	auto air_layer = std::make_shared<AirLayerHandler>();

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

void LayerGenerator::processChunk(Chunk& chunk, const HeightMap& height_map, const BlockMap& block_map)
{
	//if (isBelowSurface(chunk, height_map))
	//{
	//	chunk.setIsVisible(false);
	//	//chunk.getBlockArray().fill(Block::STONE);
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

bool LayerGenerator::isBelowSurface(Chunk& chunk, const HeightMap& height_map)
{
	int block_amount = chunk.getLevelOfDetail().block_amount;
	double min_height = 99999;
	for (int x = 0; x < block_amount; x++)
	{
		for (int z = 0; z < block_amount; z++)
		{
			min_height = std::min(min_height, height_map[x][z]);
		}
	}
	// Real CHUNK_SIZE here is correct
	int chunk_pos_y = chunk.getPos().y * CHUNK_SIZE;
	LOG_F(INFO, "chunk at pos: (%d, %d, %d) below surface: %d", chunk.getPos().x, chunk.getPos().y, chunk.getPos().z, chunk_pos_y < min_height - CHUNK_SIZE);
	return chunk_pos_y + CHUNK_SIZE < min_height;
}