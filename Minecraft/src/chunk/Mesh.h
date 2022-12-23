#pragma once
#include "../Loader.h"
#include "MeshData.h"

class Mesh
{
public:
	Mesh();
	~Mesh() = default;
	void clear();
	void addVertex(float x, float y, float z);
	void addTexture(float u, float v, float id);
	void addShading(float s);

	void loadMesh();

	const MeshData& getMesh() const;
	const Loader& getLoader() const;
	const std::vector<float>& getMeshVertexPositions() const;
	const std::vector<float>& getMeshTexturesPositions() const;
	const std::vector<float>& getMeshShadingPositions() const;
	
private:
	Loader m_loader;
	MeshData m_mesh_data;
	bool m_is_buffer_loaded;
	std::vector<float> m_vertex_positions;
	std::vector<float> m_textures_positions;
	std::vector<float> m_shading_positions;
};