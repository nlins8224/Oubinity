#pragma once
#include "Renderer.h"
#include "../shader/WaterShader.h"
#include "../gpu_loader/WaterLoader.h"
#include "../loguru.hpp"

class WaterRenderer : public Renderer
{
public:
	WaterRenderer() = delete;
	WaterRenderer(Shader shader, GLuint water_texture_id, float water_height, int water_area);
	virtual ~WaterRenderer() = default;
	void render(Camera& camera);
	void draw();
	WaterLoader& getWaterLoader();
private:
	WaterLoader m_water_loader;
	GLuint m_water_texture_id;
	bool m_is_loaded{ false };

	float m_water_height;
	int m_water_area;
};