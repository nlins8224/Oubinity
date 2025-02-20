#include "Tree.h"

Tree::Tree(uint8_t crown_height, uint8_t crown_width)
    : CROWN_HEIGHT{crown_height},
      CROWN_WIDTH{crown_width},
      m_branch_generator{} {}

bool Tree::addTree(Chunk& chunk, glm::ivec3 block_pos) {
  std::vector<ProceduralTree::Branch> branches =
      m_branch_generator.generateBranches(block_pos);
  voxelizeBranches(chunk, branches);
  addCrowns(chunk, branches);
  return true;
}

void Tree::addCrown(Chunk& chunk, glm::ivec3 block_pos) {
  int crown_width_halved = CROWN_WIDTH / 2;
  int odd_remainder = CROWN_WIDTH % 2;
  for (uint8_t y = block_pos.y; y < block_pos.y + CROWN_HEIGHT; y++) {
    for (int x = -crown_width_halved; x < crown_width_halved + odd_remainder;
         x++) {
      for (int z = -crown_width_halved; z < crown_width_halved + odd_remainder;
           z++) {
        if (shouldCutBlock(x, y, z)) continue;

        placeBlock(chunk, {block_pos.x + x, y, block_pos.z + z},
                   Block::OAK_LEAVES);
      }
    }
  }
}

void Tree::addCrowns(Chunk& chunk,
                     std::vector<ProceduralTree::Branch> branches) {
  for (ProceduralTree::Branch& branch : branches) {
    if (branch.u->childs.size() == 0) {
      addCrown(chunk, branch.u->pos);
    }
    if (branch.v->childs.size() == 0) {
      addCrown(chunk, branch.v->pos);
    }
  }
}

void Tree::voxelizeBranches(Chunk& chunk,
                            std::vector<ProceduralTree::Branch> branches) {
  for (ProceduralTree::Branch& branch : branches) {
    voxelizeBranch(chunk, branch);
  }
}

void Tree::voxelizeBranch(Chunk& chunk, ProceduralTree::Branch branch) {
  /*
   * A Fast Voxel Traversal Algorithm for Ray Tracing method is used in adapted
   * version to voxelize branch. http://www.cs.yorku.ca/~amana/research/grid.pdf
   */
  using ProceduralTree::Branch, ProceduralTree::Node;
  glm::vec3 v_pos = glm::floor(branch.v->pos);
  glm::vec3 u_pos = glm::floor(branch.u->pos);
  uint16_t dx = std::abs(u_pos.x - v_pos.x);
  uint16_t dy = std::abs(u_pos.y - v_pos.y);
  uint16_t dz = std::abs(u_pos.z - v_pos.z);

  int step_X = 1;
  int step_Y = 1;
  int step_Z = 1;

  if (dx == 0) {
    step_X = 0;
    dx = 1;
  }
  if (dy == 0) {
    step_Y = 0;
    dy = 1;
  }
  if (dz == 0) {
    step_Z = 0;
    dz = 1;
  }

  if (v_pos.x > u_pos.x) {
    std::swap(v_pos.x, u_pos.x);
  }
  if (v_pos.y > u_pos.y) {
    std::swap(v_pos.y, u_pos.y);
  }
  if (v_pos.z > u_pos.z) {
    std::swap(v_pos.z, u_pos.z);
  }

  uint16_t length = glm::length(u_pos - v_pos);

  double t_max_X = length * 0.5 / dx;
  double t_max_Y = length * 0.5 / dy;
  double t_max_Z = length * 0.5 / dz;

  double t_delta_X = length / dx;
  double t_delta_Y = length / dy;
  double t_delta_Z = length / dz;

  while (v_pos.x < u_pos.x || v_pos.y < u_pos.y || v_pos.z < u_pos.z) {
    if (t_max_X < t_max_Y) {
      if (t_max_X < t_max_Z) {
        v_pos.x += step_X;
        t_max_X += t_delta_X;
      } else {
        v_pos.z += step_Z;
        t_max_Z += t_delta_Z;
      }
    } else {
      if (t_max_Y < t_max_Z) {
        v_pos.y += step_Y;
        t_max_Y += t_delta_Y;
      } else {
        v_pos.z += step_Z;
        t_max_Z += t_delta_Z;
      }
    }
    placeBlock(chunk, v_pos, Block::OAK_LOG);
  }
}

// May overflow when near INT_MAX
// b parameter has to be positive
inline int roundDownDivide(int a, int b) {
  if (a >= 0)
    return a / b;
  else
    return (a - b + 1) / b;
}

// true modulo instead of C++ remainder modulo
inline int getMod(int pos, int mod) { return ((pos % mod) + mod) % mod; }

inline std::weak_ptr<Chunk> findNeighborAt(glm::ivec3 target_chunk_pos,
                             const ChunkNeighbors& chunk_neighbors) {
  for (const auto& [chunk_pos, chunk] : chunk_neighbors) {
    if (target_chunk_pos == chunk_pos) {
      return chunk;
    }
  }
  return {};
}

void Tree::placeBlock(Chunk& chunk, glm::ivec3 block_pos,
                      Block::block_id block_type) {
  int x, y, z;

  x = block_pos.x;
  y = block_pos.y;
  z = block_pos.z;

  glm::ivec3 chunk_pos = chunk.getPos();
  LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();

  int x_offset = roundDownDivide(x, lod.block_amount);
  int y_offset = roundDownDivide(y, lod.block_amount);
  int z_offset = roundDownDivide(z, lod.block_amount);

  if (x_offset == 0 && y_offset == 0 && z_offset == 0) {
    chunk.setBlock({x, y, z}, block_type);
    return;
  } 
  chunk_pos.x += x_offset;
  chunk_pos.y += y_offset;
  chunk_pos.z += z_offset;

  ChunkNeighbors& chunk_neighbors = chunk.getNeighbors();
  std::weak_ptr<Chunk> chunk_neighbor = findNeighborAt(chunk_pos, chunk_neighbors);
  if (!chunk_neighbor.lock()) {
    return;
  }

  // Do not set trees across lod
  if (chunk_neighbor.lock()->getLevelOfDetail().level !=
      chunk.getLevelOfDetail().level) {
    return;
  }

  x = getMod(x, lod.block_amount);
  y = getMod(y, lod.block_amount);
  z = getMod(z, lod.block_amount);

  chunk_neighbor.lock()->setBlock({x, y, z}, block_type);
}

int Tree::determineChunkOffset(int block_pos) {
  if (block_pos < 0) return (block_pos / Settings::CHUNK_SIZE) - 1;
  return block_pos / Settings::CHUNK_SIZE;
}

int Tree::determineBlockOffset(int block_pos) {
  if (block_pos < 0)
    return Settings::CHUNK_SIZE + ((block_pos) % Settings::CHUNK_SIZE);
  return block_pos % Settings::CHUNK_SIZE;
}

// Some programmer art to make trees look better
bool Tree::shouldCutBlock(int x, int y, int z) {
  int crown_width_halved = CROWN_WIDTH / 2;

  if ((x == -crown_width_halved && z == -crown_width_halved) ||
      (x == -crown_width_halved && z == crown_width_halved) ||
      (x == crown_width_halved && z == -crown_width_halved) ||
      (x == crown_width_halved && z == crown_width_halved)) {
    return true;
  }

  if (((x == -crown_width_halved && z == -crown_width_halved + 1) ||
       (x == -crown_width_halved && z == crown_width_halved - 1) ||
       (x == -crown_width_halved + 1 && z == -crown_width_halved) ||
       (x == -crown_width_halved + 1 && z == crown_width_halved) ||
       (x == crown_width_halved - 1 && z == -crown_width_halved) ||
       (x == crown_width_halved - 1 && z == crown_width_halved) ||
       (x == crown_width_halved && z == -crown_width_halved + 1) ||
       (x == crown_width_halved && z == crown_width_halved - 1))) {
    return true;
  }

  return false;
}