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
	std::vector<Face> faces;
	std::vector<Vertex> vertices;
	uint64_t col_face_masks[(CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * 6];
	uint64_t merged_forward[(CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)];
	uint64_t merged_right[(CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)];
};

struct FaceCornersAo {
	uint8_t top_left;
	uint8_t top_right;
	uint8_t bottom_right;
	uint8_t bottom_left;
};


// unordered_map is not used here, because it takes too much memory space
using ChunkNeighbors = std::vector<std::pair<glm::ivec3, Chunk*>>;

enum class ChunkState
{
	NONE = 0,
	NEW,
	CREATED,
	DECORATED,
	MESHED,
	ALLOCATED
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
	glm::ivec3 getPos() const;
	glm::ivec2 getPosXZ() const;
	Block::block_id getBlockId(glm::ivec3 block_pos) const;
	ChunkNeighbors& getNeighbors();
	bool isTransparent(glm::ivec3 block_pos) const;
	bool isVisible() const;
	void setIsVisible(bool is_visible);
	std::vector<Vertex>& getMesh();
	Block::PaletteBlockStorage& getBlockArray();
	void setBlockArray();
	const glm::vec3 getWorldPos() const;
	LevelOfDetail::LevelOfDetail getLevelOfDetail();
	unsigned int getAddedFacesAmount();
	sul::dynamic_bitset<> getBlocksBitset();
	std::vector<Face>& getFaces();
	void setState(ChunkState state);
	void setNeighbors(ChunkNeighbors neighbors);

private:
	MeshData m_mesh;
	Block::PaletteBlockStorage* m_blocks; // deleted after it's unpacked
	std::vector<Face> m_faces;
	glm::ivec3 m_chunk_pos;
	glm::vec3 m_world_pos;
	bool m_is_visible;
	LevelOfDetail::LevelOfDetail m_lod;
	unsigned int m_added_faces{ 0 };

	void addFaces();
	bool isFaceVisible(glm::ivec3 block_pos) const;
	bool isNeighborFaceVisible(glm::ivec3 block_pos) const;
	void addFace(Block::block_mesh face_side, glm::ivec3 block_pos);
	void addFaceBasedOnFaceDirection(int direction, int right, int forward, int bit_pos);
	const int get_axis_i(const int axis, const int a, const int b, const int c);
	FaceCornersAo calculateAmbientOcclusion(Block::block_mesh face_side, glm::ivec3 block_pos);
	FaceCornersAo calculateAoPlaneX(glm::ivec3 block_pos);
	FaceCornersAo calculateAoPlaneY(glm::ivec3 block_pos);
	FaceCornersAo calculateAoPlaneZ(glm::ivec3 block_pos);
	ChunkNeighbors m_chunk_neighbors;
	ChunkState m_state;
};
