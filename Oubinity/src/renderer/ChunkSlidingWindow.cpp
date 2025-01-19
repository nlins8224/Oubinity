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

WindowMovementDirection ChunkSlidingWindow::moveWindow(
    ChunkBorder chunk_border) {
  WindowMovementDirection move;
  move.x_p = m_chunk_border.min_x < chunk_border.min_x;
  move.x_n = m_chunk_border.min_x > chunk_border.min_x;
  move.z_p = m_chunk_border.min_z < chunk_border.min_z;
  move.z_n = m_chunk_border.min_z > chunk_border.min_z;

  m_chunk_border = chunk_border;
  return move;
}

void ChunkSlidingWindow::set(glm::ivec3 chunk_pos, std::shared_ptr<Chunk> chunk) {
  int index = calculateIndex(chunk_pos);
  std::weak_ptr<Chunk> current_chunk = get(chunk_pos);
  m_chunks_window[index] = chunk;
  if (int current_use_count = current_chunk.use_count() > 0) {
    LOG_F(ERROR, "chunk_pos=(%d, %d, %d), use_count=%d", chunk_pos.x,
          chunk_pos.y, chunk_pos.z, current_use_count);
  }

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
