#include "Renderer.h"

Renderer::Renderer(Shader shader)
	: m_shader{shader}
{
}

Shader& Renderer::getShader()
{
	return m_shader;
}
