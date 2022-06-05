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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Loader::bindVAO() {
	glBindVertexArray(m_vao);
}

void Loader::unbindVAO() {
	glBindVertexArray(0);
}