#pragma once
#include <glad/glad.h>
#include "../chunk/Vertex.h"
#include "DAIC.h"
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

class VertexPool
{
public:
	VertexPool();
	void draw(GLsizei draw_count);
	void updateBuffer(std::vector<Vertex>& mesh, std::vector<DAIC>& daic);
private:
	void formatVBO();

	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_daicbo;
	size_t m_vertices_amount;
};