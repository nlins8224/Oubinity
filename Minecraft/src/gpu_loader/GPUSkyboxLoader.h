#pragma once
#include <glad/glad.h>
#include <array>
#include "VAOBinder.h"
#include "../skybox/SkyboxMesh.h"

class GPUSkyboxLoader
{
public:
	void load();
	void bindVAO() const;
	GLuint getID();
private:
	GLuint m_vao, m_vbo;
	VAOBinder m_vao_binder;
};