#pragma once
#include <unordered_map>
#include "Chunk.h"
#include <iostream>


class ChunkManager
{
public:
	ChunkManager(Shader shader);
	~ChunkManager();
	void generateWorld();
	std::unordered_map<chunk_pos, Chunk, chunk_pos_hasher> getChunks();

private:
	 std::unordered_map<chunk_pos, Chunk, chunk_pos_hasher> m_chunks;
	 Shader m_shader;
};

