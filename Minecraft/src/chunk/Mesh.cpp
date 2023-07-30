#include "Mesh.h"

void Mesh::addVertex(Vertex vertex)
{
	m_mesh_data.emplace_back(vertex);
}

void Mesh::addMesh(std::vector<Vertex> mesh)
{
	m_mesh_data.insert(m_mesh_data.end(), mesh.begin(), mesh.end());
}

std::vector<Vertex>& Mesh::getMeshData() 
{
	return m_mesh_data;
}

std::vector<Vertex> Mesh::getMeshDataCopy()
{
	return m_mesh_data;
}

const MeshState Mesh::getMeshState() const
{
	return m_mesh_state;
}

void Mesh::setMeshState(MeshState state)
{
	m_mesh_state = state;
}
