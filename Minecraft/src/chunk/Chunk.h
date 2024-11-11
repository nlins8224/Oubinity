#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <functional>
#include "Vertex.h"
#include "Face.h"
#include "ChunkSize.h"
#include "ChunksMap.h"
#include "../block/Block.h"
#include "../block/BlockMesh.h"
#include "../block/BlockArray.h"
#include "../block/PaletteBlockStorage.h"
#include "../shader/Shader.h"
#include "../level_of_detail/LevelOfDetail.h"
#include "../loguru.hpp"
#include "../dynamic_bitset.hpp"
#include "../third_party/timer.h"

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
	 /        / |
	/        /  |
   2--------3   |
   |        |   |
   |  4-----|---5  --> X
   | /      |  /
   |/       | /
   0--------1
  /
 /
Z
*/

struct MeshData {
	uint64_t col_face_masks[(CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * 6]{0};
	uint64_t merged_forward[(CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)]{0};
	uint64_t merged_right[(CHUNK_SIZE + 2)]{0};
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

// unordered_map is not used here, because it takes too much memory space
using ChunkNeighbors = std::vector<std::pair<glm::ivec3, Chunk*>>;

enum class ChunkState
{
	NONE = 0,
	NEW,
	CREATED,
	NEIGHBORS_POPULATED,
	TERRAIN_GENERATED,
	DECORATED,
	MESHED,
	ALLOCATED
};

static const glm::ivec2 ao_dirs[8] = {
  glm::ivec2(-1, 0),
  glm::ivec2(0, -1),
  glm::ivec2(0, 1),
  glm::ivec2(1, 0),
  glm::ivec2(-1, -1),
  glm::ivec2(-1, 1),
  glm::ivec2(1, -1),
  glm::ivec2(1, 1),
};

class Chunk
{
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

	glm::ivec3 getPos() const;
	glm::ivec2 getPosXZ() const;
	const glm::vec3 getWorldPos() const;
	Block::block_id getBlockId(glm::ivec3 block_pos) const;
	LevelOfDetail::LevelOfDetail getLevelOfDetail();
	ChunkNeighbors& getNeighbors();
	std::vector<Vertex>& getMesh();
	std::vector<Face>& getFaces();
	Block::PaletteBlockStorage& getBlockArray();
	unsigned int getAddedFacesAmount();

	bool isTransparent(glm::ivec3 block_pos) const;
	bool isVisible() const;
	bool isBlockPresent(glm::ivec3 block_pos) const;
	bool isBlockOutsideChunk(glm::ivec3 block_pos) const;

	Chunk* findNeighborChunk(glm::ivec3 block_pos) const;
	glm::ivec3 findNeighborBlockPos(glm::ivec3 block_pos) const;

private:
	MeshData* m_mesh;
	Block::PaletteBlockStorage* m_blocks; // deleted after it's unpacked
	std::vector<Face> m_faces;
	std::vector<Vertex> m_vertices;
	glm::ivec3 m_chunk_pos;
	glm::vec3 m_world_pos;
	bool m_is_visible;
	LevelOfDetail::LevelOfDetail m_lod;
	unsigned int m_added_faces{ 0 };

	void addFaces();
	bool isFaceVisible(glm::ivec3 block_pos) const;
	bool isNeighborBlockVisible(glm::ivec3 block_pos) const;
	void addGreedyFace(GreedyQuad greedy_quad, Block::block_mesh face_side, Block::block_id type, FaceCornersAo ao);
	const uint64_t get_axis_i(const int axis, const int x, const int y, const int z);
	ChunkNeighbors m_chunk_neighbors;
	ChunkState m_state;

	const int vertexAO(uint8_t side_first, uint8_t side_second, uint8_t corner);
	const bool compareAO(const std::vector<Block::block_id>& voxels, int axis, int forward, int right, int c, int forward_offset, int right_offset);
	FaceCornersAo bakeAO(const std::vector<Block::block_id>& voxels, uint64_t bit_pos, int air_dir, uint64_t axis, uint64_t right, uint64_t forward);
};
