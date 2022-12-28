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
class Chunk;

struct ChunkNeighbors
{
	const Chunk* m_xp; // positive X chunk neighbor
	const Chunk* m_xn; // negative X chunk neighbor
	const Chunk* m_yp; // etc.
	const Chunk* m_yn;
	const Chunk* m_zp;
	const Chunk* m_zn;

	ChunkNeighbors(Chunk* xp, Chunk* xn, Chunk* yp, Chunk* yn, Chunk* zp, Chunk* zn)
		: m_xp{ xp }, m_xn{ xn }, m_yp{ yp }, m_yn{ yn }, m_zp{ zp }, m_zn{ zn } {};

	ChunkNeighbors()
	{
		m_xp = nullptr;
		m_xn = nullptr;
		m_yp = nullptr;
		m_yn = nullptr;
		m_zp = nullptr;
		m_zn = nullptr;
	}

	~ChunkNeighbors()
	{
		delete m_xp;
		delete m_xn;
		delete m_yp;
		delete m_yn;
		delete m_zp;
		delete m_zn;
	}
};

class Chunk
{
public:

	Chunk(glm::ivec3 chunk_pos, ChunkManager* chunk_manager);
	Chunk(const Chunk& chunk);
	Chunk() = default;
	~Chunk();

	void prepareChunkMesh();
	void setBlock(glm::ivec3 block_pos, Block::block_id type);
	glm::ivec3 getPosition() const;
	Block::block_id getBlockId(glm::ivec3 block_pos) const;
	bool isTransparent(glm::ivec3 block_pos) const;
	bool isMeshLoaded() const;
	bool isTerrainGenerated() const;
	bool isChunkVisible() const;
	void setIsMeshLoaded(bool is_loaded);
	void setIsTerrainGenerated(bool is_generated);
	void setIsChunkVisible(bool is_visible);
	const Mesh& getMesh() const;
	Mesh& getMesh();

private:
	// TODO: m_chunk_manager should be const ref?
	ChunkManager* m_chunk_manager;
	Mesh m_mesh;
	Block::BlockArray m_blocks;
	ChunkNeighbors m_neighbors;
	glm::ivec3 m_chunk_pos;
	glm::ivec3 m_world_pos;
	bool m_is_mesh_buffer_loaded;
	bool m_is_terrain_generated;
	bool m_is_chunk_visible;

	void addChunkMesh();
	void addVisibleFaces(glm::ivec3 block_pos);
	bool isFaceVisible(glm::ivec3 world_pos) const;
	void addFace(Block::block_mesh face_side, glm::ivec3 block_pos);
};
