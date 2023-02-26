#include "ChunkLayerHandler.h"

bool ChunkLayerHandler::handle(Chunk& chunk, const HeightMap& height_map, int seed)
{
	if (tryHandle(chunk, height_map, seed))
		return true;

	if (m_next != nullptr)
		return m_next->handle(chunk, height_map, seed);

	return false;
}