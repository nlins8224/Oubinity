#pragma once
#include "HeightMap.h"
#include "../chunk/Chunk.h"

class ChunkLayerHandler
{
public:
	virtual bool handle(Chunk& chunk, const HeightMap& height_map, int seed);

private:
	virtual bool tryHandle(Chunk& chunk, const HeightMap& height_map, int seed) = 0;
	std::unique_ptr<ChunkLayerHandler> m_next{ nullptr };
};
