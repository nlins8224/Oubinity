#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "ChunkRendererSettings.h"

class Chunk;

class ChunkSlidingWindow
{
public:
	ChunkSlidingWindow() = default;
	virtual ~ChunkSlidingWindow() = default;
	void set(glm::ivec3 chunk_pos, Chunk* chunk);
	Chunk* get(glm::ivec3 chunk_pos);
private:
	glm::ivec3 calculateIndex(glm::ivec3 chunk_pos);
	Chunk* m_chunks_window[ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS][ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS][ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS];

	// true modulo instead of C++ remainder modulo
	inline int getMod(int pos, int mod)
	{
		return ((pos % mod) + mod) % mod;
	}
};