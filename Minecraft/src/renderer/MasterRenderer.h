#pragma once
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "../Camera.h"
#include "ChunkRenderer.h"

class MasterRenderer
{
public:
	MasterRenderer();
	~MasterRenderer() = default;
	void initConfig() const;
	void clear() const;
	void render(Camera& camera, ChunksMap* chunks_map);

private:
	ChunkRenderer m_chunk_renderer;
};

