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

class Chunk
{
public:
	static const int CHUNK_SIZE{ 16 };
	Chunk(chunk_pos position);
	Chunk(const Chunk& chunk);
	Chunk() = default;
	~Chunk() = default;

	void updateChunk();
	void prepareChunkMesh();
	void loadChunkMesh();
	void renderChunk();
	void setBlock(int x, int y, int z, Block::block_id type);
	chunk_pos getChunkPos();

private:
	bool m_has_mesh{ false };
	std::vector<float> m_mesh_vertex_positions;
	chunk_pos m_chunk_position{0, 0, 0};
	Loader m_loader;
	std::array<std::array<std::array<int, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_SIZE> m_blocks{ 0 };

	// Later xyz could be changed to chunk_pos
	void addVisibleFaces(int x, int y, int z);
	bool isFaceVisible(int x, int y, int z);
	void addFace(std::array<float, BlockMesh::FACE_SIZE> const &face, int x, int y, int z);
	int getBlockId(int x, int y, int z);
};
