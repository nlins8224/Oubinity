#include "ChunkMeshLoader.h"

ChunkMeshLoader::ChunkMeshLoader()
{
	// TODO initializer list?
	m_vao = 0;
	m_vertex_vbo = 0;
	m_texture_vbo = 0;
	m_shading_vbo = 0;
	m_xyzs_vbo = 0;
	m_uvw_vbo = 0;
}

void ChunkMeshLoader::loadMesh(const MeshData& mesh)
{
	createVAO();
	storeDataInVAO(mesh);
	unbindVAO();
}

void ChunkMeshLoader::loadPackedMesh(const PackedMeshData& packed_mesh)
{
	createVAO();
	storePackedDataInVAO(packed_mesh);
	unbindVAO();
}

void ChunkMeshLoader::createVAO() {
	if (m_vao != 0)
		cleanBuffers();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
}

void ChunkMeshLoader::storeDataInVAO(const MeshData& mesh) {
	glGenBuffers(1, &m_vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertex_positions.size() * sizeof(float), mesh.vertex_positions.data(), GL_STATIC_DRAW);

	// store xyz position coords
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glGenBuffers(1, &m_texture_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_texture_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.textures_positions.size() * sizeof(float), mesh.textures_positions.data(), GL_STATIC_DRAW);
	// store uvw textures coords
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	//store shading values
	glGenBuffers(1, &m_shading_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_shading_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.shading_positions.size() * sizeof(float), mesh.shading_positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
}

void ChunkMeshLoader::storePackedDataInVAO(const PackedMeshData& packed_mesh)
{
	// store xyzs
	glGenBuffers(1, &m_xyzs_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_xyzs_vbo);
	glBufferData(GL_ARRAY_BUFFER, packed_mesh.xyzs_positions.size() * sizeof(GLuint), packed_mesh.xyzs_positions.data(), GL_STATIC_DRAW);

	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// store uvw
	glGenBuffers(1, &m_uvw_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvw_vbo);
	glBufferData(GL_ARRAY_BUFFER, packed_mesh.uvw_positions.size() * sizeof(GLuint), packed_mesh.uvw_positions.data(), GL_STATIC_DRAW);

	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1);
}

void ChunkMeshLoader::bindVAO() const {
	m_vao_binder.bindVAO(m_vao);
}

void ChunkMeshLoader::unbindVAO() const {
	m_vao_binder.unbindVAO();
}

void ChunkMeshLoader::cleanPackedBuffers() const
{
	glDeleteBuffers(1, &m_xyzs_vbo);
	glDeleteBuffers(1, &m_uvw_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

void ChunkMeshLoader::cleanBuffers() const {

	glDeleteBuffers(1, &m_vertex_vbo);
	glDeleteBuffers(1, &m_texture_vbo);
	glDeleteBuffers(1, &m_shading_vbo);
	glDeleteVertexArrays(1, &m_vao);
}