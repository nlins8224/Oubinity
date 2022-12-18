#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <functional>
#include "Mesh.h"
#include "../block/Block.h"
#include "../block/BlockMesh.h"
#include "../Loader.h"
#include "../shader/Shader.h"
#include "../TextureManager.h"
#include "optick.h"

class ChunkManager;

/* 
Convention: 
world_pos is what it seems to be
chunk_pos is position of a chunk in the world, calculated by: floor(world_pos) / CHUNK_SIZE 
block_pos is position of a block inside the chunk, calculated by: floor(world_pos) % CHUNK_SIZE
*/

class Chunk
{
public:
	static const int CHUNK_SIZE_X{ 16 };
	static const int CHUNK_SIZE_Y{ 64 };
	static const int CHUNK_SIZE_Z{ 16 };

	Chunk(glm::ivec3 chunk_pos, ChunkManager* chunk_manager);
	Chunk(const Chunk& chunk);
	Chunk() = default;
	~Chunk() = default;

	void prepareChunkMesh();
	void renderChunk();
	void setBlock(glm::ivec3 block_pos, Block::block_id type);
	glm::ivec3 getPosition();
	Block::block_id getBlockId(glm::ivec3 block_pos);
	bool isTransparent(glm::ivec3 block_pos);
	bool isMeshLoaded();
	bool isTerrainGenerated();
	void setIsMeshLoaded(bool is_loaded);
	void setIsTerrainGenerated(bool is_generated);
	std::array<std::array<std::array<Block::block_id, CHUNK_SIZE_X>, CHUNK_SIZE_Y>, CHUNK_SIZE_Z> m_blocks{ Block::AIR };
	Mesh& getMesh();

private:
	// TODO: Move initialization to constructor with default value
	glm::ivec3 m_chunk_pos{ 0, 0, 0 };
	glm::ivec3 m_world_pos;
	Mesh m_mesh;
	ChunkManager* m_chunk_manager;
	// TODO: Move initialization to constructor with default value
	bool m_is_mesh_buffer_loaded{ false };
	// TODO: Move initialization to constructor with default value
	bool m_is_terrain_generated{ false };

	void addChunkMesh();
	void loadChunkMesh();
	void addVisibleFaces(glm::ivec3 block_pos);
	bool isFaceVisible(glm::ivec3 world_pos);
	void addFace(Block::block_mesh face_side, glm::ivec3 block_pos);
};
