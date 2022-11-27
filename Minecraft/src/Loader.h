#pragma once
#include <vector>
#include <glad/glad.h>
#include <optick.h>
#include "Mesh.h"

class Loader 
{
public:
	void loadMesh(Mesh& mesh);
	void bindVAO();
	Loader() = default;
	~Loader() = default;

private:
	GLuint m_vao;
	GLuint m_vertex_vbo, m_texture_vbo, m_shading_vbo;

	void createVAO();
	void storeDataInVAO(int attribute_number, Mesh& mesh);
	void unbindVAO();
};