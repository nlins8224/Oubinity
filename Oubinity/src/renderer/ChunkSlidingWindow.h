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
  void set(glm::ivec3 chunk_pos, std::shared_ptr<Chunk> chunk);
  std::weak_ptr<Chunk> get(glm::ivec3 chunk_pos);
  ChunkBorder getBorder();
 private:
  int calculateIndex(glm::ivec3 chunk_pos);
  std::vector<std::shared_ptr<Chunk>> m_chunks_window;
  std::queue<WindowMovementDirection> m_pending_movement_actions;

  ChunkBorder m_chunk_border;
};