#include "Mesh.h"

Mesh::Mesh()
{
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

void Mesh::addPacked_xyzs(glm::ivec3 xyz, float s)
{
	GLuint packed_xyzs = ChunkMeshPacker::pack_xyzs(xyz, s);
	m_packed_mesh_data.xyzs_positions.emplace_back(packed_xyzs);
}

void Mesh::addPacked_uvw(glm::ivec3 uvw)
{
	GLuint packed_uvw = ChunkMeshPacker::pack_uvw(uvw);
	m_packed_mesh_data.uvw_positions.emplace_back(packed_uvw);
}

void Mesh::addPacked_xyzs(GLubyte x, GLubyte y, GLubyte z, GLubyte s)
{
	GLuint packed_xyzs = ChunkMeshPacker::pack_xyzs(x, y, z, s);
	m_packed_mesh_data.xyzs_positions.emplace_back(packed_xyzs);
}

void Mesh::addPacked_uvw(GLubyte u, GLubyte v, GLubyte w)
{
	GLuint packed_uvw = ChunkMeshPacker::pack_uvw(u, v, w);
	m_packed_mesh_data.uvw_positions.emplace_back(packed_uvw);
}

const MeshData& Mesh::getMesh() const 
{
	return m_mesh_data;
}

const PackedMeshData& Mesh::getPackedMesh() const
{
	return m_packed_mesh_data;
}

const Loader& Mesh::getLoader() const
{
	return m_loader;
}

const std::vector<float>& Mesh::getMeshVertexPositions() const
{
	return m_mesh_data.vertex_positions;
}

const std::vector<float>& Mesh::getMeshTexturesPositions() const
{
	return m_mesh_data.textures_positions;
}

const std::vector<float>& Mesh::getMeshShadingPositions() const
{
	return m_mesh_data.shading_positions;
}

const float Mesh::getTrianglesCount() const
{
	return m_packed_mesh_data.xyzs_positions.size();
}

void Mesh::loadMesh()
{
	m_loader.loadMesh(m_mesh_data);
}

void Mesh::loadPackedMesh()
{
	m_loader.loadPackedMesh(m_packed_mesh_data);
}
