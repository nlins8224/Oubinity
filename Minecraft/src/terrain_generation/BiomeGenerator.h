#pragma once
#include <array>
#include "../chunk/Chunk.h"

using height_map = std::array<std::array<uint8_t, CHUNK_SIZE_X>, CHUNK_SIZE_Z>;

class BiomeGenerator
{
public:
	BiomeGenerator() = default;
	~BiomeGenerator() = default;
	void processChunk(Chunk& chunk, const height_map& height_map);
private:
	double m_noise_factor;
	int m_water_threshold;
};