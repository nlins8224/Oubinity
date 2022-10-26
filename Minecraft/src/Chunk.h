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
	void setBlock(uint8_t x, uint8_t y, uint8_t z, Block::block_id type);

private:
	std::vector<float> m_mesh_vertex_positions;
	std::vector<float> m_mesh_textures_positions;
	glm::ivec3 m_chunk_position{0, 0, 0};
	Loader m_loader;
	TextureManager* m_texture_manager;
	// block_id should be here instead of int?
	std::array<std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_SIZE> m_blocks{ 0 };

	// Later xyz could be changed to chunk_pos
	void addVisibleFaces(uint8_t x, uint8_t y, uint8_t z);
	bool isFaceVisible(int8_t x, int8_t y, int8_t z);
	void addFace(std::array<float, BlockMesh::FACE_SIZE> const &face, int x, int y, int z);
	int setFaceTexture(int8_t block_id);
	int getBlockId(uint8_t x, uint8_t y, uint8_t z);
};
