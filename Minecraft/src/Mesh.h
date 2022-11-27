#pragma once
#include <vector>

struct Mesh
{
	std::vector<float> vertex_positions;
	std::vector<float> textures_positions;
	std::vector<float> shading_positions;

	inline void clear()
	{
		vertex_positions.clear();
		textures_positions.clear();
		shading_positions.clear();
	}

	inline void addVertex(float x, float y, float z)
	{
		vertex_positions.emplace_back(x);
		vertex_positions.emplace_back(y);
		vertex_positions.emplace_back(z);
	}

	inline void addTexture(float u, float v, float id)
	{
		textures_positions.emplace_back(u);
		textures_positions.emplace_back(v);
		textures_positions.emplace_back(id);
	}

	inline void addShading(float s)
	{
		shading_positions.emplace_back(s);
	}
};