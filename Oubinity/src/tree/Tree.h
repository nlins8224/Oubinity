#pragma once
#include <glm/glm.hpp>
#include <unordered_set>
#include "../chunk/Chunk.h"
#include "../chunk/ChunksMap.h"  // Include this for glm::ivec3 hash, TODO: Refactor
#include "../renderer/ChunkSlidingWindow.h"
#include "BranchGenerator.h"

class Tree {
 public:
  Tree(uint8_t crown_height, uint8_t crown_width);
  virtual ~Tree() = default;
  bool spawnTree(Chunk& chunk,
                 const std::vector<ProceduralTree::Branch>& branches,
                 glm::vec3 spawn_block_pos, ChunkSlidingWindow& chunk_sliding_widnow);

 private:
  void spawnCrown(Chunk& chunk, glm::ivec3 block_pos, glm::vec3 spawn_block_pos,
                  ChunkSlidingWindow& chunk_sliding_widnow);
  void spawnCrowns(Chunk& chunk, const std::vector<ProceduralTree::Branch>& branches,
                   glm::vec3 spawn_block_pos,
                   ChunkSlidingWindow& chunk_sliding_widnow);
  void voxelizeBranches(Chunk& chunk,
                        const std::vector<ProceduralTree::Branch>& branches,
                        glm::vec3 spawn_block_pos,
                        ChunkSlidingWindow& chunk_sliding_widnow);
  void voxelizeBranch(Chunk& chunk, ProceduralTree::Branch branch,
                      glm::vec3 spawn_block_pos,
                      ChunkSlidingWindow& chunk_sliding_widnow);
  void placeBlock(Chunk& chunk, glm::ivec3 block_pos,
                  Block::block_id block_type,
                  ChunkSlidingWindow& chunk_sliding_widnow);
  int determineChunkOffset(int block_pos);
  int determineBlockOffset(int block_pos);
  bool shouldCutBlock(int x, int y, int z);
  uint8_t CROWN_HEIGHT;
  uint8_t CROWN_WIDTH;
};