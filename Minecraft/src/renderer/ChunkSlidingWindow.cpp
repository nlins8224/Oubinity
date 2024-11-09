#include "ChunkSlidingWindow.h"

void ChunkSlidingWindow::set(glm::ivec3 chunk_pos, Chunk* chunk)
{
	glm::ivec3 index = calculateIndex(chunk_pos);
	m_chunks_window[index.x][index.y][index.z] = chunk;
}

Chunk* ChunkSlidingWindow::get(glm::ivec3 chunk_pos)
{
	glm::ivec3 index = calculateIndex(chunk_pos);
	return m_chunks_window[index.x][index.y][index.z];
}

glm::ivec3 ChunkSlidingWindow::calculateIndex(glm::ivec3 chunk_pos)
{
	int M_XZ{ ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS };
	int M_Y{ ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS };
	int x{ chunk_pos.x }, y{ chunk_pos.y }, z{ chunk_pos.z };

	glm::ivec3 chunks_window_index {
		getMod(x, M_XZ),
		getMod(y, M_Y),
		getMod(z, M_XZ)
	};

	return chunks_window_index;
}
