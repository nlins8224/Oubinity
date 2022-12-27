#pragma once
#include "Chunk.h"
#include "ChunkSize.h"
#include "../third_party/FastNoiseLite.h"
#include <optick.h>

using height_map = std::array<std::array<uint8_t, CHUNK_SIZE_X>, CHUNK_SIZE_Z>;

class WorldGenerator
{
public:
	WorldGenerator(int world_seed);
	~WorldGenerator() = default;
	void generateChunkTerrain(Chunk& chunk);
private:
	height_map generateChunkHeightMap(glm::ivec3 chunk_pos, const int world_seed) const;
	int m_world_seed;
};

