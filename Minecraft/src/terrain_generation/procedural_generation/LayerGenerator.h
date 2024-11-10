 #pragma once
#include <array>
#include "../../Settings.h"
#include "../../chunk/Chunk.h"
#include "../../loguru.hpp"
#include "../TerrainGenerationTypes.h"

class LayerGenerator
{
public:
	LayerGenerator(int seed, uint8_t water_height);
	~LayerGenerator() = default;
	bool processChunk(Chunk& chunk, const HeightMap& height_map);
private:
	int m_seed;
	uint8_t m_water_height;
};