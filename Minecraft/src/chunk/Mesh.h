#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Vertex.h"

enum class MeshState
{
	NONE = 0,
	NEW,
	READY,
	READY_TO_DECORATE,
	DECORATED,
	PROCESSED
};

class Mesh
{
public:
	Mesh() = default;
	~Mesh() = default;

	void addVertex(Vertex vertex);
	void addMesh(std::vector<Vertex> mesh);
	
	std::vector<Vertex>& getMeshData();
	std::vector<Vertex> getMeshDataCopy();

	const MeshState getMeshState() const;
	void setMeshState(MeshState state);
	
private:
	std::vector<Vertex> m_mesh_data;
	MeshState m_mesh_state{MeshState::NEW};
};