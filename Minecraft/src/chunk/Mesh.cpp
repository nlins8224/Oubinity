#include "Mesh.h"

Mesh::Mesh()
{
	m_is_buffer_loaded = false;
}

void Mesh::clear()
{
	m_mesh_data.vertex_positions.clear();
	m_mesh_data.textures_positions.clear();
	m_mesh_data.shading_positions.clear();
}

void Mesh::addVertex(float x, float y, float z)
{
	m_mesh_data.vertex_positions.emplace_back(x);
	m_mesh_data.vertex_positions.emplace_back(y);
	m_mesh_data.vertex_positions.emplace_back(z);
}

void Mesh::addTexture(float u, float v, float id)
{
	m_mesh_data.textures_positions.emplace_back(u);
	m_mesh_data.textures_positions.emplace_back(v);
	m_mesh_data.textures_positions.emplace_back(id);
}

void Mesh::addShading(float s)
{
	m_mesh_data.shading_positions.emplace_back(s);
}

MeshData& Mesh::getMesh()
{
	return m_mesh_data;
}

Loader Mesh::getLoader()
{
	return m_loader;
}

std::vector<float>& Mesh::getMeshVertexPositions()
{
	return m_mesh_data.vertex_positions;
}

std::vector<float>& Mesh::getMeshTexturesPositions()
{
	return m_mesh_data.textures_positions;
}

std::vector<float>& Mesh::getMeshShadingPositions()
{
	return m_mesh_data.shading_positions;
}

void Mesh::loadMesh()
{
	if (m_is_buffer_loaded)
		return;

	m_loader.loadMesh(m_mesh_data);
	m_is_buffer_loaded = true;
}
