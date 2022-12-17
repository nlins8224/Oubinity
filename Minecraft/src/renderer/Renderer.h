#pragma once
#include "../Camera.h"
#include "../Loader.h"
#include "../Mesh.h"
#include "../Shader.h"

class Renderer
{
public:
	Renderer(Shader shader);
	~Renderer() = default;
	virtual void render(Camera& camera) = 0;
protected:
	Shader m_shader;
private:
	virtual void draw(Mesh& mesh) const = 0;
};

