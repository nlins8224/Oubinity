#include "ChunkSlidingWindow.h"

ChunkSlidingWindow::ChunkSlidingWindow()
	: m_chunk_border{}
{
	int chunks = ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS;
	m_chunks_window.resize(chunks);
}

ChunkSlidingWindow::ChunkSlidingWindow(ChunkBorder chunk_border)
	: m_chunk_border{chunk_border}
{
	int chunks = ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS;
	m_chunks_window.resize(chunks);
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
	int index = calculateIndex(chunk_pos);
	m_chunks_window[index] = chunk;
}

Chunk* ChunkSlidingWindow::get(glm::ivec3 chunk_pos)
{
	int index = calculateIndex(chunk_pos);
	return m_chunks_window[index];
}

void ChunkSlidingWindow::setBorder(ChunkBorder chunk_border)
{
	m_chunk_border = chunk_border;
}

ChunkBorder ChunkSlidingWindow::getBorder()
{
	return m_chunk_border;
}

int ChunkSlidingWindow::calculateIndex(glm::ivec3 chunk_pos)
{
	int C_XZ{ ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS };
	int C_Y{ ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS };
	int x{ chunk_pos.x }, y{ chunk_pos.y }, z{ chunk_pos.z };

	glm::ivec3 mod_index {
		getMod(x, C_XZ),
		getMod(y, C_Y),
		getMod(z, C_XZ)
	};
	return (mod_index.x * C_XZ * C_Y) + (mod_index.y * C_XZ) + mod_index.z;
}
