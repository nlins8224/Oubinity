#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <functional>
#include "Mesh.h"
#include "ChunkSize.h"
#include "../block/Block.h"
#include "../block/BlockMesh.h"
#include "../block/BlockArray.h"
#include "../Loader.h"
#include "../shader/Shader.h"
#include "../TextureManager.h"
#include "optick.h"

/*
Convention:
world_pos is what it seems to be
chunk_pos is position of a chunk in the world, calculated by: floor(world_pos) / CHUNK_SIZE
block_pos is position of a block inside the chunk, calculated by: floor(world_pos) % CHUNK_SIZE
*/

class ChunkManager;

class Chunk
{
public:

	Chunk(glm::ivec3 chunk_pos, ChunkManager* chunk_manager);
	Chunk(const Chunk& chunk);
	Chunk() = default;
	~Chunk();

	void addChunkMesh();
	void setBlock(glm::ivec3 block_pos, Block::block_id type);
	glm::ivec3 getPosition() const;
	Block::block_id getBlockId(glm::ivec3 block_pos) const;
	bool isTransparent(glm::ivec3 block_pos) const;
	bool isTerrainGenerated() const;
	void setIsTerrainGenerated(bool is_generated);
	const Mesh& getMesh() const;
	Mesh& getMesh();
	const glm::vec3 getWorldPos() const;

private:
	// TODO: m_chunk_manager should be const ref?
	ChunkManager* m_chunk_manager;
	Mesh m_mesh;
	Block::BlockArray m_blocks;
	glm::ivec3 m_chunk_pos;
	glm::vec3 m_world_pos;
	bool m_is_terrain_generated;

	void addVisibleFaces(glm::ivec3 block_pos);
	bool isFaceVisible(glm::ivec3 world_pos) const;
	void addFace(Block::block_mesh face_side, glm::ivec3 block_pos);
};
