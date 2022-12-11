#pragma once
#include "Chunk.h"
#include "FastNoiseLite.h"
#include <iostream>
#include <optick.h>

using height_map = std::array<std::array<uint8_t, Chunk::CHUNK_SIZE_X>, Chunk::CHUNK_SIZE_Z>;

class WorldGenerator
{
public:
	height_map generateChunkHeightMap(glm::ivec3 chunk_pos, const int world_seed) const;
};

