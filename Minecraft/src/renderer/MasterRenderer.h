#pragma once
#include <shared_mutex>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "ChunkRenderer.h"
#include "SkyboxRenderer.h"
#include "GradientRenderer.h"
#include "../Camera.h"

class MasterRenderer
{
public:
	MasterRenderer(Camera& camera, GLuint skybox_texture_id, GLuint texture_array_id);
	~MasterRenderer() = default;
	void initConfig();
	void clear() const;
	void render(Camera& camera);
	ChunkRenderer& getChunkRenderer();
	SkyboxRenderer& getSkyboxRenderer();
	GradientRenderer& getGradientRenderer();

private:
	ChunkRenderer m_chunk_renderer;
	SkyboxRenderer m_skybox_renderer;
	GradientRenderer m_gradient_renderer;
};

