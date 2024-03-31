#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <functional>
#include "Mesh.h"
#include "Vertex.h"
#include "Face.h"
#include "ChunkSize.h"
#include "ChunksMap.h"
#include "../block/Block.h"
#include "../block/BlockMesh.h"
#include "../block/BlockArray.h"
#include "../block/PaletteBlockStorage.h"
#include "../block/PaletteChunkView.h"
#include "../shader/Shader.h"
#include "../level_of_detail/LevelOfDetail.h"
#include "../loguru.hpp"

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


struct FaceCornersAo {
	uint8_t top_left;
	uint8_t top_right;
	uint8_t bottom_right;
	uint8_t bottom_left;
};

using ChunkNeighbors = std::unordered_map<glm::ivec3, Chunk*>;

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
	bool isTerrainGenerated() const;
	void setIsTerrainGenerated(bool is_generated);
	bool isVisible() const;
	void setIsVisible(bool is_visible);
	Mesh& getMesh();
	Block::PaletteChunkView& getBlockArray();
	const glm::vec3 getWorldPos() const;
	LevelOfDetail::LevelOfDetail getLevelOfDetail();
	unsigned int getAddedFacesAmount();
	std::vector<Face>& getFaces();
	void setState(ChunkState state);
	void setNeighbors(ChunkNeighbors neighbors);

private:
	Mesh m_mesh;
	Block::PaletteChunkView* m_blocks;
	std::vector<Face> m_faces;
	glm::ivec3 m_chunk_pos;
	glm::vec3 m_world_pos;
	bool m_is_terrain_generated;
	bool m_is_visible;
	LevelOfDetail::LevelOfDetail m_lod;
	unsigned int m_added_faces{ 0 };

	bool isFaceVisible(glm::ivec3 world_pos) const;
	void addVisibleFaces(glm::ivec3 block_pos);
	void addFace(Block::block_mesh face_side, glm::ivec3 block_pos);
	FaceCornersAo calculateAmbientOcclusion(Block::block_mesh face_side, glm::ivec3 block_pos);
	FaceCornersAo calculateAoPlaneX(glm::ivec3 block_pos);
	FaceCornersAo calculateAoPlaneY(glm::ivec3 block_pos);
	FaceCornersAo calculateAoPlaneZ(glm::ivec3 block_pos);
	ChunkNeighbors m_chunk_neighbors;
	ChunkState m_state;
};
