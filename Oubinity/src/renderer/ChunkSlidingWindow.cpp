#include "ChunkSlidingWindow.h"
#include "../Util.h"
#include "../loguru.hpp"

ChunkSlidingWindow::ChunkSlidingWindow() : m_chunk_border{} {
  int chunks = Settings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS *
               Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS *
               Settings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS;
  m_chunks_window.resize(chunks);
}

ChunkSlidingWindow::ChunkSlidingWindow(ChunkBorder chunk_border)
    : m_chunk_border{chunk_border} {
  int chunks = Settings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS *
               Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS *
               Settings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS;
  m_chunks_window.resize(chunks);
}

void ChunkSlidingWindow::moveWindow(
    ChunkBorder chunk_border) {
  LOG_F(INFO, "Chunk Border min_x=%d, max_x=%d, min_z=%d, max_z=%d",
        chunk_border.min_x, chunk_border.max_x, chunk_border.min_z,
        chunk_border.max_z);
  m_chunk_border = chunk_border;
}

WindowMovementDirection ChunkSlidingWindow::getWindowLatestMoveDir(
    ChunkBorder chunk_border) {
  WindowMovementDirection move;
  move.x_p = m_chunk_border.min_x < chunk_border.min_x;
  move.x_n = m_chunk_border.min_x > chunk_border.min_x;
  move.z_p = m_chunk_border.min_z < chunk_border.min_z;
  move.z_n = m_chunk_border.min_z > chunk_border.min_z;
  return move;
}

void ChunkSlidingWindow::set(glm::ivec3 chunk_pos, std::shared_ptr<Chunk> chunk) {
  int index = calculateIndex(chunk_pos);
  m_chunks_window[index] = chunk;
}

std::weak_ptr<Chunk> ChunkSlidingWindow::get(glm::ivec3 chunk_pos) {
  int index = calculateIndex(chunk_pos);
  return m_chunks_window[index];
}

ChunkBorder ChunkSlidingWindow::getBorder() { return m_chunk_border; }

int ChunkSlidingWindow::calculateIndex(glm::ivec3 chunk_pos) {
  int C_XZ{Settings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS};
  int C_Y{Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS};
  int x{chunk_pos.x}, y{chunk_pos.y}, z{chunk_pos.z};

  glm::ivec3 mod_index{Util::getMod(x, C_XZ), Util::getMod(y, C_Y),
                       Util::getMod(z, C_XZ)};
  int index = (mod_index.x * C_XZ * C_Y) + (mod_index.y * C_XZ) + mod_index.z;
  return index;
}
