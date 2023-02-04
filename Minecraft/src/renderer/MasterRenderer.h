#pragma once
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "ChunkRenderer.h"
#include "../Camera.h"

class MasterRenderer
{
public:
	MasterRenderer(ChunksMap& chunks_map);
	~MasterRenderer() = default;
	void initConfig() const;
	void clear() const;
	void render(Camera& camera);

private:
	ChunkRenderer m_chunk_renderer;
};

