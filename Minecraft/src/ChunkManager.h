#pragma once
#include <unordered_map>
#include "Chunk.h"
#include "TextureManager.h"
#include <iostream>


class ChunkManager
{
public:
	ChunkManager();
	~ChunkManager() = default;
	void generateWorld();
	std::unordered_map<chunk_pos, Chunk, chunk_pos_hasher> getChunks();
	TextureManager textureManager;

private:
	 std::unordered_map<chunk_pos, Chunk, chunk_pos_hasher> m_chunks;
};

