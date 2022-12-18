#pragma once
#include <vector>
#include <glad/glad.h>
#include <optick.h>
#include "chunk/MeshData.h"


class Loader 
{
public:
	Loader() = default;
	~Loader() = default;
	void loadMesh(MeshData& mesh);
	void bindVAO();
	
private:
	GLuint m_vao;
	GLuint m_vertex_vbo, m_texture_vbo, m_shading_vbo;

	void createVAO();
	void storeDataInVAO(int attribute_number, MeshData& mesh);
	void unbindVAO();
};