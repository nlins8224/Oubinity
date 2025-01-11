#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>
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
  WindowMovementDirection moveWindow(ChunkBorder chunk_border);
  void set(glm::ivec3 chunk_pos, Chunk* chunk);
  Chunk* get(glm::ivec3 chunk_pos);
  void setBorder(ChunkBorder chunk_border);
  ChunkBorder getBorder();
 private:
  int calculateIndex(glm::ivec3 chunk_pos);
  std::vector<Chunk*> m_chunks_window;
  ChunkBorder m_chunk_border;
};