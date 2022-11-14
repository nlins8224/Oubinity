#include "Loader.h"
#include <iostream>


void Loader::loadMesh(std::vector<float>& mesh_vertices, std::vector<float>& mesh_textures, std::vector<float>& mesh_shades)
{
	createVAO();
	storeDataInVAO(0, mesh_vertices, mesh_textures, mesh_shades);
	unbindVAO();
}

void Loader::createVAO() {
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
}

void Loader::storeDataInVAO(int attribute_number, std::vector<float>& mesh_vertices, std::vector<float>& mesh_textures, std::vector<float>& mesh_shades) {
	glGenBuffers(1, &m_vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_vertices.size() * sizeof(float), mesh_vertices.data(), GL_STATIC_DRAW);

	// store xyz position coords
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glGenBuffers(1, &m_texture_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_texture_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_textures.size() * sizeof(float), mesh_textures.data(), GL_STATIC_DRAW);
	// store uvw textures coords
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	//store shading values
	glGenBuffers(1, &m_shading_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_shading_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_shades.size() * sizeof(float), mesh_shades.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
}

void Loader::bindVAO() {
	glBindVertexArray(m_vao);
}

void Loader::unbindVAO() {
	glBindVertexArray(0);
}