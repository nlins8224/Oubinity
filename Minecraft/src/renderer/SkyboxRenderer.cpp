#include "SkyboxRenderer.h"

SkyboxRenderer::SkyboxRenderer(Shader shader, GLuint skybox_texture_id)
	: Renderer(shader),
	m_skybox_texture_id{skybox_texture_id}
{
}

void SkyboxRenderer::render(Camera& camera)
{
	//glDepthFunc(GL_LEQUAL);
	m_shader.bind();

	// remove translation from view matrix by casting it out
	glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
	glm::mat4 projection = camera.getProjectionMatrix();
	m_shader.setUniformMat4("view", view);
	m_shader.setUniformMat4("projection", projection);

	m_skybox_loader.bindVAO();
	draw();
	glBindVertexArray(0);
	//glDepthFunc(GL_LESS);
}

void SkyboxRenderer::draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox_texture_id);
	m_shader.setUniformFloat("u_size", Skybox::SIZE);

	const GLuint skybox_triangles_amount = Skybox::vertices_amount / 3;
	glDrawArrays(GL_TRIANGLES, 0, skybox_triangles_amount);
}

GPUSkyboxLoader& SkyboxRenderer::getSkyboxLoader()
{
	return m_skybox_loader;
}
