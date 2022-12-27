#include "Loader.h"

Loader::Loader()
{
	m_vao = 0;
	m_vertex_vbo = 0;
	m_texture_vbo = 0;
	m_shading_vbo = 0;
}

void Loader::loadMesh(const MeshData& mesh)
{
	createVAO();
	storeDataInVAO(mesh);
	unbindVAO();
}

void Loader::createVAO() {
	if (m_vao != 0)
		cleanBuffers();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
}

void Loader::storeDataInVAO(const MeshData& mesh) {
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

void Loader::bindVAO() const {
	glBindVertexArray(m_vao);
}

void Loader::unbindVAO() const {
	glBindVertexArray(0);
}

void Loader::cleanBuffers() const {

	glDeleteBuffers(1, &m_vertex_vbo);
	glDeleteBuffers(1, &m_texture_vbo);
	glDeleteBuffers(1, &m_shading_vbo);
	glDeleteVertexArrays(1, &m_vao);
}