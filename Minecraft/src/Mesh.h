#pragma once
#include <vector>

struct Mesh
{
	std::vector<float> vertex_positions;
	std::vector<float> textures_positions;
	std::vector<float> shading_positions;

	void clear()
	{
		vertex_positions.clear();
		textures_positions.clear();
		shading_positions.clear();
	}
};