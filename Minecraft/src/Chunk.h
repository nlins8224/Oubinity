#pragma once
#include "Block.h"
#include "BlockMesh.h"
#include "Loader.h"
#include <glm/glm.hpp>
#include <vector>
#include <array>

// This should be splitted later into Chunk and ChunkRenderer classes
class Chunk
{
public:
	static const int CHUNK_SIZE{ 16 };
	Chunk(glm::ivec3 position);
	Chunk(const Chunk& chunk);
	Chunk() = default;

	void updateChunk();
	void setBlock(int x, int y, int z, Block::block_id type);
	void prepareChunkMesh();
	void loadChunkMesh();
	void renderChunk();
private:
	bool m_has_mesh{ false };
	Loader m_chunk_loader;
	std::vector<float> m_mesh_vertex_positions;
	glm::ivec3 m_chunk_position;

	// This could be one array later
	std::array<std::array<std::array<int, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_SIZE> m_blocks;

	void addVisibleFaces(int x, int y, int z);
	bool isFaceVisible(int x, int y, int z);
	void addFace(std::array<float, 30> const &face, int x, int y, int z);
	void initAirChunk();
	int getBlockId(int x, int y, int z);

};

