#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <functional>
#include "Mesh.h"
#include "VertexCompresser.h"
#include "ChunkSize.h"
#include "ChunksMap.h"
#include "../block/Block.h"
#include "../block/BlockMesh.h"
#include "../block/BlockArray.h"
#include "../shader/Shader.h"
#include "../TextureManager.h"
#include "../level_of_detail/LevelOfDetail.h"


/*
Convention:
world_pos is what it seems to be
chunk_pos is position of a chunk in the world, calculated by: floor(world_pos) / CHUNK_SIZE
block_pos is position of a block inside the chunk, calculated by: floor(world_pos) % CHUNK_SIZE
*/

class Chunk
{
public:

	Chunk(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod);
	Chunk(const Chunk& chunk);
	Chunk() = default;
	~Chunk() = default;

	void addChunkMesh();
	void addChunkDecorationMesh();
	void setBlock(glm::ivec3 block_pos, Block::block_id type);
	glm::ivec3 getPos() const;
	glm::ivec2 getPosXZ() const;
	Block::block_id getBlockId(glm::ivec3 block_pos) const;
	bool isTransparent(glm::ivec3 block_pos) const;
	bool isTerrainGenerated() const;
	void setIsTerrainGenerated(bool is_generated);
	Mesh& getMesh();
	std::array<std::array<std::array<Block::block_id, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_SIZE>& getBlockArray();
	const glm::vec3 getWorldPos() const;
	LevelOfDetail::LevelOfDetail getLevelOfDetail();
	unsigned int getAddedFacesAmount();
private:
	Mesh m_mesh;
	glm::ivec3 m_chunk_pos;
	glm::vec3 m_world_pos;
	bool m_is_terrain_generated;
	LevelOfDetail::LevelOfDetail m_lod;
	std::array<std::array<std::array<Block::block_id, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_SIZE> m_blocks{ Block::AIR };
	unsigned int m_added_faces{ 0 };

	bool isFaceVisible(glm::ivec3 world_pos) const;
	void addVisibleFaces(glm::ivec3 block_pos);
	void addFace(Block::block_mesh face_side, glm::ivec3 block_pos);
};
