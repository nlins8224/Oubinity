#pragma once
#include <vector>

struct MeshData
{
	std::vector<float> vertex_positions;
	std::vector<float> textures_positions;
	std::vector<float> shading_positions;
};

struct PackedMeshData
{
	std::vector<GLuint> xyzs_positions;
	std::vector<GLuint> uvw_positions;
};
