 #pragma once
#include <array>
#include "layers/AirLayerHandler.h"
#include "layers/SurfaceLayerHandler.h"
#include "layers/SurfacePreloadedLayerHandler.h"
#include "layers/UndergroundLayerHandler.h"
#include "layers/OceanLayerHandler.h"
#include "../../chunk/Chunk.h"
#include "../../loguru.hpp"
#include "../TerrainGenerationTypes.h"

class LayerGenerator
{
public:
	LayerGenerator(int seed, uint8_t water_height);
	~LayerGenerator() = default;
	bool processChunk(Chunk& chunk, const HeightMap& height_map);
	bool processChunk(Chunk& chunk, const HeightMap& height_map, const BlockMap& block_map);
private:
	bool isBlockInSurfaceHeightBounds(glm::ivec3 block_pos, glm::ivec3 chunk_pos, int surface_height, int block_size);
	int m_seed;
	uint8_t m_water_height;
};