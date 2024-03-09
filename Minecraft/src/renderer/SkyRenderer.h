#pragma once
#include "Renderer.h"
#include "../shader/SkyShader.h"
#include "../gpu_loader/SkyLoader.h"
#include "../loguru.hpp"

class SkyRenderer : public Renderer {
public:
	SkyRenderer() = delete;
	SkyRenderer(Shader shader, GLuint cloud_noise_texture_id, glm::vec2 resolution);
	virtual ~SkyRenderer() = default;
	void render(Camera& camera);
	void draw();
	void setTimeElapsed(float time_in_sec);
	void setMousePos(glm::vec2 mouse_pos);
	SkyLoader& getSkyLoader();
private:
	SkyLoader m_sky_loader;
	GLuint m_cloud_noise_texture_id;
	glm::vec2 m_resolution;
	float m_time_elapsed_sec;
	glm::vec2 m_mouse_pos;
};