#include "ChunkSlidingWindow.h"

ChunkSlidingWindow::ChunkSlidingWindow()
	: m_chunk_border{}
{

}

ChunkSlidingWindow::ChunkSlidingWindow(ChunkBorder chunk_border)
	: m_chunk_border{chunk_border}
{
}

WindowMovementDirection ChunkSlidingWindow::moveWindow(ChunkBorder chunk_border)
{
	WindowMovementDirection move;
	move.x_p = m_chunk_border.min_x < chunk_border.min_x;
	move.x_n = m_chunk_border.min_x > chunk_border.min_x;
	move.z_p = m_chunk_border.min_z < chunk_border.min_z;
	move.z_n = m_chunk_border.min_z > chunk_border.min_z;

	m_chunk_border = chunk_border;
	return move;
}

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

void ChunkSlidingWindow::setBorder(ChunkBorder chunk_border)
{
	m_chunk_border = chunk_border;
}

ChunkBorder ChunkSlidingWindow::getBorder()
{
	return m_chunk_border;
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
