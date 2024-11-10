#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "ChunkRendererSettings.h"
#include "ChunkBorder.h"

class Chunk;

struct WindowMovementDirection
{
	bool x_n;
	bool x_p;
	bool z_n;
	bool z_p;
};

class ChunkSlidingWindow
{
public:
	ChunkSlidingWindow();
	ChunkSlidingWindow(ChunkBorder chunk_border);
	virtual ~ChunkSlidingWindow() = default;
	WindowMovementDirection moveWindow(ChunkBorder chunk_border);
	void set(glm::ivec3 chunk_pos, Chunk* chunk);
	Chunk* get(glm::ivec3 chunk_pos);
	void setBorder(ChunkBorder chunk_border);
	ChunkBorder getBorder();
private:
	glm::ivec3 calculateIndex(glm::ivec3 chunk_pos);
	//TODO: Allocate this on heap
	Chunk* m_chunks_window[ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS][ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS][ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS];
	ChunkBorder m_chunk_border;

	// true modulo instead of C++ remainder modulo
	inline int getMod(int pos, int mod)
	{
		return ((pos % mod) + mod) % mod;
	}
};