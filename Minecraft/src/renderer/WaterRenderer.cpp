#include "WaterRenderer.h"

WaterRenderer::WaterRenderer(Shader shader, GLuint water_texture_id, float water_height, int water_area)
	: Renderer(shader),
	m_water_texture_id{water_texture_id},
	m_water_height{water_height},
	m_water_area{water_area}
{
}

void WaterRenderer::render(Camera& camera)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_shader.bind();
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = camera.getProjectionMatrix();
	m_shader.setUniformMat4("view", view);
	m_shader.setUniformMat4("projection", projection);
	m_shader.setUniformInt("water_area", m_water_area);
	m_shader.setUniformFloat("water_height", m_water_height);

	m_water_loader.bindVAO();
	draw();
	glBindVertexArray(0);
	glDisable(GL_BLEND);
}

void WaterRenderer::draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_water_texture_id);

	const GLuint water_triangles_amount = Water::VERTICES_AMOUNT;
	glDrawArrays(GL_TRIANGLES, 0, water_triangles_amount);
}

WaterLoader& WaterRenderer::getWaterLoader()
{
	return m_water_loader;
}
