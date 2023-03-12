#pragma once
#include <array>
#include "NoiseMapTypes.h"
#include "layers/AirLayerHandler.h"
#include "layers/SurfaceLayerHandler.h"
#include "layers/UndergroundLayerHandler.h"
#include "../chunk/Chunk.h"

class BiomeGenerator
{
public:
	BiomeGenerator(int seed);
	~BiomeGenerator() = default;
	void processChunk(Chunk& chunk, const HeightMap& height_map);
private:
	int m_seed;
};