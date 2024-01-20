#include "ChunkLayerHandler.h"

bool ChunkLayerHandler::handle(Chunk& chunk, glm::ivec3 block_pos, int surface_height, int seed)
{
	if (tryHandle(chunk, block_pos, surface_height, seed))
		return true;

	if (m_next != nullptr)
		return m_next->handle(chunk, block_pos, surface_height, seed);

	return false;
}

std::shared_ptr<ChunkLayerHandler> ChunkLayerHandler::addNextLayer(std::shared_ptr<ChunkLayerHandler> next)
{
	m_next = next;
	return m_next;
}
