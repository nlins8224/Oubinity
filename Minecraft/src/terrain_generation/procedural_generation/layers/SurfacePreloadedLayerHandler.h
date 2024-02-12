#pragma once
#include "ChunkLayerHandler.h"
#include "../../../loguru.hpp"
#include "../../TerrainGenerationTypes.h"

class SurfacePreloadedLayerHandler : public ChunkLayerHandler
{
public:
	SurfacePreloadedLayerHandler(BlockMap block_map);
	virtual ~SurfacePreloadedLayerHandler() = default;
	bool tryHandle(Chunk& chunk, glm::ivec3 block_pos, int surface_height, int seed) override;
private:
	BlockMap m_block_map;
};