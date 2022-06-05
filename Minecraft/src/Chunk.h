#pragma once
#include "Block.h"
#include "BlockMesh.h"
#include "Loader.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <functional>

struct chunk_pos
{
	int x, y, z;

	chunk_pos(int _x, int _y, int _z) : x{ _x }, y{ _y }, z{ _z } {}

	bool operator==(const chunk_pos& other) const
	{
		return (x == other.x
			&& y == other.y
			&& z == other.z);
	}
};

struct chunk_pos_hasher
{
	size_t operator()(const chunk_pos& p) const
	{
		size_t res = 17;
		res = res * 31 + std::hash<int>()(p.x);
		res = res * 31 + std::hash<int>()(p.y);
		res = res * 31 + std::hash<int>()(p.z);
		return res;
	}
};

// This should be splitted later into Chunk and ChunkRenderer classes
class Chunk
{
public:
	static const int CHUNK_SIZE{ 16 };
	Chunk(chunk_pos position, Shader& shader);
	Chunk(const Chunk& chunk);
	Chunk() = default;
	~Chunk() = default;

	void updateChunk();
	void setBlock(int x, int y, int z, Block::block_id type);
	void prepareChunkMesh();
	void loadChunkMesh();
	void renderChunk();
	chunk_pos getChunkPos();
	std::array<std::array<std::array<int, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_SIZE> getBlocks();

private:
	bool m_has_mesh{ false };
	Shader m_shader{ "shaders/blockVertex.glsl", "shaders/blockFragment.glsl" };
	std::vector<float> m_mesh_vertex_positions;
	chunk_pos m_chunk_position{0, 0, 0}; // <- to default constructor?
	Loader m_loader;
	// This could be one array later
	std::array<std::array<std::array<int, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_SIZE> m_blocks;

	void addVisibleFaces(int x, int y, int z);
	bool isFaceVisible(int x, int y, int z);
	void addFace(std::array<float, 30> const &face, int x, int y, int z);
	void initAirChunk();
	int getBlockId(int x, int y, int z);

};

