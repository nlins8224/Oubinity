#pragma once
#include "../Camera.h"
#include "../Loader.h"
#include "../Mesh.h"

class Renderer
{
public:
	Renderer() = default;
	~Renderer() = default;
	virtual void render(Camera& camera) = 0;
private:
	virtual void draw(Mesh& mesh) const = 0;
};

