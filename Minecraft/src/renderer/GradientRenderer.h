#pragma once

#include "Renderer.h"
#include "../shader/GradientShader.h"
#include "../gpu_loader/GPUSkyboxLoader.h"

class GradientRenderer : public Renderer
{
public:
	GradientRenderer() = delete;
	GradientRenderer(Shader shader);
	~GradientRenderer() = default;
	void render(Camera& camera);
	void draw();
	GPUSkyboxLoader& getGradientLoader();
private:
	// re-use SkyboxLoader as GradientLoader
	GPUSkyboxLoader m_gradient_loader;
	bool is_loaded{ false };
};