#pragma once
#include <array>
#include "NoiseMapTypes.h"
#include "layers/AirLayerHandler.h"
#include "layers/SurfaceLayerHandler.h"
#include "layers/UndergroundLayerHandler.h"
#include "layers/OceanLayerHandler.h"
#include "../chunk/Chunk.h"

class BiomeGenerator
{
public:
	BiomeGenerator(int seed, uint8_t surface_height, uint8_t water_height);
	~BiomeGenerator() = default;
	void processChunk(Chunk& chunk, const NoiseMap& height_map);
	bool isBelowSurface(uint8_t height);
private:
	int m_seed;
	uint8_t m_min_surface_height;
	uint8_t m_water_height;
};