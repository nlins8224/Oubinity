#pragma once
#include <glm/glm.hpp>
#include "../Loader.h"
#include "MeshData.h"
#include "ChunkMeshPacker.h"

enum class MeshState
{
	READY = 0,
	PROCESSED,
	LOADED
};

class Mesh
{
public:
	Mesh();
	~Mesh() = default;
	void clear();
	void addVertex(float x, float y, float z);
	void addTexture(float u, float v, float id);
	void addShading(float s);

	void addPacked_xyzs(glm::ivec3 xyz, float s);
	void addPacked_uvw(glm::ivec3 uvw);

	void addPacked_xyzs(GLubyte x, GLubyte y, GLubyte z, GLubyte s);
	void addPacked_uvw(GLubyte u, GLubyte v, GLubyte w);

	void loadMesh();
	void loadPackedMesh();

	const MeshData& getMesh() const;
	const PackedMeshData& getPackedMesh() const;
	const Loader& getLoader() const;
	const std::vector<float>& getMeshVertexPositions() const;
	const std::vector<float>& getMeshTexturesPositions() const;
	const std::vector<float>& getMeshShadingPositions() const;

	const float getTrianglesCount() const;
	const MeshState getMeshState() const;
	void setMeshState(MeshState state);
	
private:
	Loader m_loader;
	MeshData m_mesh_data;
	PackedMeshData m_packed_mesh_data;
	MeshState m_mesh_state;
};