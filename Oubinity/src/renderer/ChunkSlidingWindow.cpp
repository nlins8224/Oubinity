#include "ChunkSlidingWindow.h"
#include "../Util.h"
#include "../loguru.hpp"
#include "../chunk/Chunk.h"

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

WindowMovementDirection ChunkSlidingWindow::getWindowMoveDir(
    ChunkBorder src_border, ChunkBorder dst_border) {
  WindowMovementDirection move;
  move.x_p = src_border.min_x < dst_border.min_x;
  move.x_n = src_border.min_x > dst_border.min_x;
  move.z_p = src_border.min_z < dst_border.min_z;
  move.z_n = src_border.min_z > dst_border.min_z;
  return move;
}

void ChunkSlidingWindow::set(glm::ivec3 chunk_pos, std::shared_ptr<Chunk> chunk) {
  std::shared_ptr<Chunk> prev_chunk = get(chunk_pos).lock();
  if (prev_chunk) {
    prev_chunk->clearBlocks();
    prev_chunk->clearBlockOccupancyCache();
    prev_chunk->clearFaces();
    prev_chunk->setState(
       {.has_blocks = false, .was_edited = false, .needs_lod_update = false});
  }
  int index = calculateIndex(chunk_pos);
  m_chunks_window[index] = chunk;
}

std::weak_ptr<Chunk> ChunkSlidingWindow::get(glm::ivec3 chunk_pos) {
  int index = calculateIndex(chunk_pos);
  return m_chunks_window[index];
}

ChunkBorder ChunkSlidingWindow::getBorder() { return m_chunk_border; }

size_t ChunkSlidingWindow::closestDistanceToBorder(glm::ivec3 chunk_pos) const {
  size_t distance_x = std::numeric_limits<size_t>::max();
  size_t distance_z = std::numeric_limits<size_t>::max();
  int x = chunk_pos.x, z = chunk_pos.z;
  distance_x = std::min(std::abs(x - m_chunk_border.min_x), std::abs(x - m_chunk_border.max_x));
  distance_z = std::min(std::abs(z - m_chunk_border.min_z), std::abs(z - m_chunk_border.max_z));
  return std::min(distance_x, distance_z);
}

int ChunkSlidingWindow::calculateIndex(glm::ivec3 chunk_pos) {
  int C_XZ{Settings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS};
  int C_Y{Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS};
  int x{chunk_pos.x}, y{chunk_pos.y}, z{chunk_pos.z};

  glm::ivec3 mod_index{Util::getMod(x, C_XZ), Util::getMod(y, C_Y),
                       Util::getMod(z, C_XZ)};
  int index = (mod_index.x * C_XZ * C_Y) + (mod_index.y * C_XZ) + mod_index.z;
  return index;
}
