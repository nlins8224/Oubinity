#include "Loader.h"
#include <iostream>


void Loader::loadMesh(std::vector<float> vertices)
{
	createVAO();
	storeDataInVAO(0, vertices);
	unbindVAO();
}

void Loader::createVAO() {
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
}

void Loader::storeDataInVAO(int attribute_number, std::vector<float> vertices) {
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// store xyz position coords
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	// store uvw textures coords
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Loader::bindVAO() {
	glBindVertexArray(m_vao);
}

void Loader::unbindVAO() {
	glBindVertexArray(0);
}