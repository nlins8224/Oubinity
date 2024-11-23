#pragma once
#include "Renderer.h"
#include "../shader/SkyboxShader.h"
#include "../gpu_loader/SkyboxLoader.h"

class SkyboxRenderer : public Renderer
{
public:
	SkyboxRenderer() = delete;
	SkyboxRenderer(Shader shader, GLuint skybox_texture_id);
	~SkyboxRenderer() = default;
	void render(Camera& camera);
	void draw();
	SkyboxLoader& getSkyboxLoader();
private:
	SkyboxLoader m_skybox_loader;
	GLuint m_skybox_texture_id;
	bool is_loaded{ false };
};