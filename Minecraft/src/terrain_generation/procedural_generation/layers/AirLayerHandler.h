#pragma once
#include "ChunkLayerHandler.h"

class AirLayerHandler : public ChunkLayerHandler
{
public:
	bool tryHandle(Chunk& chunk, glm::ivec3 block_pos, int surface_height, int seed) override;

};