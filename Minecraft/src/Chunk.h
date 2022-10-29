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

class Chunk
{
public:
	static const uint8_t CHUNK_SIZE{ 16 };
	Chunk(TextureManager* texture_manager, glm::ivec3 position);
	Chunk(const Chunk& chunk);
	Chunk() = default;
	~Chunk() = default;

	void updateChunk();
	void prepareChunkMesh();
	void loadChunkMesh();
	void renderChunk();
	void setBlock(glm::ivec3 pos, Block::block_id type);
	glm::ivec3 getPosition();
	int getBlockId(glm::ivec3 pos);
	bool isTransparent(glm::ivec3 pos);
private:
	std::vector<float> m_mesh_vertex_positions;
	std::vector<float> m_mesh_textures_positions;
	glm::ivec3 m_chunk_position{0, 0, 0};
	Loader m_loader;
	TextureManager* m_texture_manager;
	// block_id should be here instead of int?
	std::array<std::array<std::array<int, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_SIZE> m_blocks{ Block::AIR };

	void addVisibleFaces(glm::ivec3 pos);
	bool isFaceVisible(glm::ivec3 pos);
	void addFace(std::array<float, BlockMesh::FACE_SIZE> const &face, glm::ivec3 pos);
	int setFaceTexture(int8_t block_id);	
};
