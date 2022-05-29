#include "Loader.h"
#include <iostream>

Loader::Loader(std::vector<float> vertices)
{
	load(vertices);
}

void Loader::load(std::vector<float> vertices)
{
	createVAO();
	storeDataInVAO(0, vertices);
	unbindVAO();
}

void Loader::createVAO() {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	m_vaos.push_back(vao);
}

void Loader::storeDataInVAO(int attribute_number, std::vector<float> vertices) {
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices.front(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	m_vbos.push_back(vbo);
}

void Loader::bindVAO(int idx) {
	glBindVertexArray(m_vaos[idx]);
}

void Loader::unbindVAO() {
	glBindVertexArray(0);
}