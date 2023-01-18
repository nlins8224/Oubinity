#pragma once
#include <vector>
#include <glad/glad.h>
#include <optick.h>
#include "chunk/MeshData.h"


class Loader 
{
public:
	Loader();
	~Loader() = default;
	void loadMesh(const MeshData& mesh);
	void bindVAO() const;
	void unbindVAO() const;
	
private:
	GLuint m_vao;
	GLuint m_vertex_vbo, m_texture_vbo, m_shading_vbo;

	void createVAO();
	void storeDataInVAO(const MeshData& mesh);
	void cleanBuffers() const;
};