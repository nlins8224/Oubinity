#pragma once
#include "../chunk/Chunk.h"
#include "../chunk/ChunkSize.h"
#include "../third_party/FastNoiseLite.h"
#include <optick.h>

using height_map = std::array<std::array<uint8_t, CHUNK_SIZE_X>, CHUNK_SIZE_Z>;

class TerrainGenerator
{
public:
	TerrainGenerator(int world_seed, const int render_distance);
	~TerrainGenerator() = default;
	void generateChunkTerrain(Chunk& chunk, const int render_distance);
private:
	height_map generateChunkHeightMap(glm::ivec3 chunk_pos, const int world_seed) const;
	int m_world_seed;
};

