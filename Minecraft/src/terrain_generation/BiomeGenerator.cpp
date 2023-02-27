#include "BiomeGenerator.h"

BiomeGenerator::BiomeGenerator(int seed)
	: m_seed{seed}
{
}

void BiomeGenerator::processChunk(Chunk& chunk, const HeightMap& height_map)
{
	auto layer_handler = std::make_shared<AirLayerHandler>();
	auto surface_layer = std::make_shared<SurfaceLayerHandler>();
	auto underground_layer = std::make_shared<UndergroundLayerHandler>();

	layer_handler->addNextLayer(surface_layer)->addNextLayer(underground_layer);

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