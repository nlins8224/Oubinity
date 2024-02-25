#pragma once
#include "Renderer.h"
#include "../shader/SkyShader.h"
#include "../gpu_loader/SkyLoader.h"
#include "../loguru.hpp"

class SkyRenderer : public Renderer {
public:
	SkyRenderer() = delete;
	SkyRenderer(Shader shader, GLuint cloud_noise_texture_id);
	virtual ~SkyRenderer() = default;
	void render(Camera& camera);
	void draw();
	SkyLoader& getSkyLoader();
private:
	SkyLoader m_sky_loader;
	GLuint m_cloud_noise_texture_id;
};