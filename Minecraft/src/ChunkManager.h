#pragma once
#include <unordered_map>
#include "Chunk.h"
#include <iostream>

inline long key(glm::ivec3 v) { return (long)v.x << 16 | (long)v.y << 8 | (long)v.z; }

class ChunkManager
{
public:
	ChunkManager();
	void generateWorld();
	std::unordered_map<long, Chunk> getChunks();

private:
	 std::unordered_map<long, Chunk> m_chunks;
};

