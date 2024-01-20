#pragma once
#include "../../../chunk/Chunk.h"

class ChunkLayerHandler
{
public:
	ChunkLayerHandler() = default;
	~ChunkLayerHandler() = default;
	virtual bool handle(Chunk& chunk, glm::ivec3 block_pos, int surface_height, int seed);
	virtual std::shared_ptr<ChunkLayerHandler> addNextLayer(std::shared_ptr<ChunkLayerHandler> next);

protected:
	virtual bool tryHandle(Chunk& chunk, glm::ivec3 block_pos, int surface_height, int seed) = 0;

private:
	std::shared_ptr<ChunkLayerHandler> m_next{ nullptr };
};
