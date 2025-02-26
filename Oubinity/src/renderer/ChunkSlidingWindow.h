#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <memory>
#include <queue>
#include "ChunkBorder.h"

class Chunk;

struct WindowMovementDirection {
  bool x_n;
  bool x_p;
  bool z_n;
  bool z_p;
};

class ChunkSlidingWindow {
 public:
  ChunkSlidingWindow();
  ChunkSlidingWindow(ChunkBorder chunk_border);
  virtual ~ChunkSlidingWindow() = default;
  void moveWindow(ChunkBorder chunk_border);
  WindowMovementDirection getWindowLatestMoveDir(ChunkBorder chunk_border);
  WindowMovementDirection getWindowMoveDir(ChunkBorder src_border,
                                           ChunkBorder dst_border);
  void set(glm::ivec3 chunk_pos, std::shared_ptr<Chunk> chunk);
  std::weak_ptr<Chunk> get(glm::ivec3 chunk_pos);
  ChunkBorder getBorder();
  size_t closestDistanceToBorder(glm::ivec3 chunk_pos) const;
 private:
  int calculateIndex(glm::ivec3 chunk_pos);
  std::vector<std::shared_ptr<Chunk>> m_chunks_window;
  ChunkBorder m_chunk_border;
};