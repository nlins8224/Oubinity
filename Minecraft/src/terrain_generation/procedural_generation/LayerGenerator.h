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
	void processChunk(Chunk& chunk, const HeightMap& height_map);
	void processChunk(Chunk& chunk, const HeightMap& height_map, const BlockMap& block_map);
	bool isBelowSurface(Chunk& chunk, const HeightMap& height_map);
private:
	int m_seed;
	uint8_t m_water_height;
};