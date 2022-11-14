#pragma once
#include "Block.h"
#include "BlockMesh.h"
#include "Loader.h"
#include "Shader.h"
#include "TextureManager.h"
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <functional>

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
	static const int CHUNK_SIZE_Y{ 256 };
	static const int CHUNK_SIZE_Z{ 16 };

	Chunk(TextureManager* texture_manager, glm::ivec3 chunk_pos);
	Chunk(const Chunk& chunk);
	Chunk() = default;
	~Chunk() = default;

	void updateChunk();
	void prepareChunkMesh();
	void loadChunkMesh();
	void renderChunk();
	void setBlock(glm::ivec3 block_pos, Block::block_id type);
	glm::ivec3 getPosition();
	Block::block_id getBlockId(glm::ivec3 block_pos);
	bool isTransparent(glm::ivec3 block_pos);
private:
	std::vector<float> m_mesh_vertex_positions;
	std::vector<float> m_mesh_textures_positions;
	std::vector<float> m_mesh_shading_positions;
	glm::ivec3 m_chunk_pos{0, 0, 0};
	Loader m_loader;
	TextureManager* m_texture_manager;
	// block_id should be here instead of int?
	std::array<std::array<std::array<Block::block_id, CHUNK_SIZE_X>, CHUNK_SIZE_Y>, CHUNK_SIZE_Z> m_blocks{ Block::AIR };

	void addVisibleFaces(glm::ivec3 block_pos);
	bool isFaceVisible(glm::ivec3 block_pos);
	void addFace(BlockMesh::block_mesh face_side, glm::ivec3 block_pos);
	int setFaceTexture(Block::block_id type);	
};
