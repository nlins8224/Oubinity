#pragma once
#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <vector>
#include "../block/Block.h"
#include "../block/BlockArray.h"
#include "../block/BlockMesh.h"
#include "../block/BlockStorage.h"
#include "../dynamic_bitset.hpp"
#include "../level_of_detail/LevelOfDetail.h"
#include "../loguru.hpp"
#include "../Settings.h"
#include "../shader/Shader.h"
#include "../third_party/timer.h"
#include "ChunksMap.h"
#include "Face.h"
#include "Vertex.h"
// clang-format off
/*
Convention:
world_pos is what it seems to be
chunk_pos is position of a chunk in the world, calculated by: floor(world_pos) / CHUNK_SIZE 
block_pos is position of a block inside the chunk, calculated by: floor(world_pos) % CHUNK_SIZE

Voxel:
* 0 - (0, 0, 1)
* 1 - (1, 0, 1)
* 2 - (0, 1, 1)
* 3 - (1, 1, 1)
* 4 - (0, 0, 0)
* 5 - (1, 0, 0)
* 6 - (0, 1, 0)
* 7 - (1, 1, 0)

     ^ Y
     |
     |
     6--------7
    /         /|
   /         / |
   2--------3  |
   |        |  |
   |  4-----|--5  --> X
   | /      |  /
   |/       | /
   0--------1
  /
 /
Z
*/
// clang-format on

struct MeshData {
  uint64_t col_face_masks[(Settings::CHUNK_SIZE + 2) *
                          (Settings::CHUNK_SIZE + 2) * 6]{0};
  uint64_t merged_forward[(Settings::CHUNK_SIZE + 2) *
                          (Settings::CHUNK_SIZE + 2)]{0};
  uint64_t merged_right[(Settings::CHUNK_SIZE + 2)]{0};
};

struct FaceCornersAo {
  GLubyte left_back;
  GLubyte right_back;
  GLubyte right_front;
  GLubyte left_front;
};

struct GreedyQuad {
  uint8_t left;
  uint8_t right;
  uint8_t front;
  uint8_t back;
  uint8_t up;
  uint8_t width;
  uint8_t height;
};

struct ChunkState {
  bool has_blocks;
  bool was_edited;

  ChunkState()
      : has_blocks{false},
        was_edited{false} {}
};

// unordered_map is not used here, because it takes too much memory space
using ChunkNeighbors = std::vector<std::pair<glm::ivec3, std::weak_ptr<Chunk>>>;

static const glm::ivec2 ao_dirs[8] = {
    glm::ivec2(-1, 0),  glm::ivec2(0, -1), glm::ivec2(0, 1),  glm::ivec2(1, 0),
    glm::ivec2(-1, -1), glm::ivec2(-1, 1), glm::ivec2(1, -1), glm::ivec2(1, 1),
};

class Chunk {
 public:
  Chunk(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod);
  Chunk(const Chunk& chunk);
  Chunk() = default;
  virtual ~Chunk();

  void addChunkMesh();

  void setBlock(glm::ivec3 block_pos, Block::block_id type);
  void setNeighbors(ChunkNeighbors neighbors);
  void setState(ChunkState state);
  void setIsVisible(bool is_visible);
  void setBlockArray();
  void setChunkHasBlocksState(bool has_blocks);
  void setChunkEditedState(bool was_edited);

  void clearFaces();

  ChunkState getState();
  glm::ivec3 getPos() const;
  glm::ivec2 getPosXZ() const;
  const glm::vec3 getWorldPos() const;
  Block::block_id getBlockId(glm::ivec3 block_pos) const;
  LevelOfDetail::LevelOfDetail getLevelOfDetail();
  ChunkNeighbors& getNeighbors();
  std::vector<Vertex>& getMesh();
  std::vector<Face>& getFaces();
  Block::BlockStorage& getBlockArray();
  unsigned int getAddedFacesAmount();
  bool isTransparent(glm::ivec3 block_pos) const;
  bool isVisible() const;
  bool isBlockPresent(glm::ivec3 block_pos) const;
  bool isBlockOutsideChunk(glm::ivec3 block_pos) const;

  std::weak_ptr<Chunk> findNeighborChunk(glm::ivec3 block_pos) const;
  glm::ivec3 findNeighborBlockPos(glm::ivec3 block_pos) const;

 private:
  MeshData* m_mesh;
  Block::BlockStorage* m_blocks;
  std::vector<Face> m_faces;
  std::vector<Vertex> m_vertices;
  glm::ivec3 m_chunk_pos;
  glm::vec3 m_world_pos;
  bool m_is_visible;
  LevelOfDetail::LevelOfDetail m_lod;
  unsigned int m_added_faces{0};

  void addFaces();
  bool isNeighborBlockVisible(glm::ivec3 block_pos) const;
  void addGreedyFace(GreedyQuad greedy_quad, Block::block_mesh face_side,
                     Block::block_id type, FaceCornersAo ao);
  const uint64_t get_axis_i(const int axis, const int x, const int y,
                            const int z);
  ChunkNeighbors m_chunk_neighbors;
  ChunkState m_state;

  const int vertexAO(uint8_t side_first, uint8_t side_second, uint8_t corner);
  const bool compareAO(const std::vector<Block::block_id>& voxels, int axis,
                       int forward, int right, int c, int forward_offset,
                       int right_offset);
  FaceCornersAo bakeAO(const std::vector<Block::block_id>& voxels,
                       uint64_t bit_pos, int air_dir, uint64_t axis,
                       uint64_t right, uint64_t forward);
};
