#include "SkyRenderer.h"

SkyRenderer::SkyRenderer(Shader shader, GLuint cloud_noise_texture_id)
	: Renderer(shader),
	m_cloud_noise_texture_id{cloud_noise_texture_id}
{
}

void SkyRenderer::render(Camera& camera)
{
	glDepthFunc(GL_LEQUAL);
	m_shader.bind();

	m_sky_loader.bindVAO();
	draw();
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

void SkyRenderer::draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_cloud_noise_texture_id);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

SkyLoader& SkyRenderer::getSkyLoader()
{
	return m_sky_loader;
}
