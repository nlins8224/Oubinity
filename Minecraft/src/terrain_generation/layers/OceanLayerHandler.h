#pragma once
#include "ChunkLayerHandler.h"

class OceanLayerHandler : public ChunkLayerHandler
{
public:
	OceanLayerHandler(int water_threshold);
	~OceanLayerHandler() = default;
	bool tryHandle(Chunk& chunk, glm::ivec3 block_pos, int surface_height, int seed) override;
private:
	int m_water_threshold;
};