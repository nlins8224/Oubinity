#pragma once
#include <unordered_map>
#include "Chunk.h"
#include "TextureManager.h"


class ChunkManager
{
public:
	ChunkManager();
	~ChunkManager() = default;
	void generateWorld();
	std::unordered_map<chunk_pos, Chunk, chunk_pos_hasher> getChunks();
	TextureManager m_texture_manager{16, 16, 256};

private:
	 std::unordered_map<chunk_pos, Chunk, chunk_pos_hasher> m_chunks;
};

