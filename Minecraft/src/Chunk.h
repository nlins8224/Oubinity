#pragma once
#include "Block.h"
#include "BlockMesh.h"
#include "Mesh.h"
#include "Loader.h"
#include "Shader.h"
#include "TextureManager.h"
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <functional>
#include "optick.h"


class ChunkManager;

struct glm_ivec3_hasher
{
	size_t operator()(const glm::ivec3& p) const
	{
		size_t res = 17;
		res = res * 31 + std::hash<int>()(p.x);
		res = res * 31 + std::hash<int>()(p.y);
		res = res * 31 + std::hash<int>()(p.z);
		return res;
	}
};

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

	//TODO: Chunk needs only ChunkManager and chunk_pos now
	Chunk(TextureManager* texture_manager, glm::ivec3 chunk_pos, ChunkManager* chunk_manager);
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
	void setIsTerrainGenerated(bool is_generated);
	std::array<std::array<std::array<Block::block_id, CHUNK_SIZE_X>, CHUNK_SIZE_Y>, CHUNK_SIZE_Z> m_blocks{ Block::AIR };
	Mesh& getMesh();
	TextureManager* getTextureManager();

private:
	glm::ivec3 m_chunk_pos{ 0, 0, 0 };
	glm::ivec3 m_world_pos;
	Mesh m_mesh;
	TextureManager* m_texture_manager;
	ChunkManager* m_chunk_manager;
	bool m_is_mesh_buffer_loaded{ false };
	bool m_is_terrain_generated{ false };

	void addChunkMesh();
	void loadChunkMesh();
	void addVisibleFaces(glm::ivec3 block_pos);
	bool isFaceVisible(glm::ivec3 world_pos);
	void addFace(BlockMesh::block_mesh face_side, glm::ivec3 block_pos);
};
