#include "ChunkRenderer.h"

void ChunkRenderer::render(Camera& camera)
{
	if (m_chunks.empty())
		return;

	m_shader.bind();
	for (Chunk* chunk : m_chunks)
	{
		renderChunk(camera, chunk);
	}
}

void ChunkRenderer::add(Chunk* chunk)
{
	m_chunks.emplace_back(chunk);
}

void ChunkRenderer::draw(Mesh& mesh) const
{
	mesh.getLoader().bindVAO();
	uint8_t vertices_per_triangle{ 3 };
	uint64_t amount_of_triangles{ mesh.getMeshVertexPositions().size() / vertices_per_triangle};
	glDrawArrays(GL_TRIANGLES, 0, amount_of_triangles);
}

void ChunkRenderer::renderChunk(Camera& camera, Chunk* chunk)
{
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = camera.getProjectionMatrix();
	m_shader.setUniformMat4("model", model);
	m_shader.setUniformMat4("view", view);
	m_shader.setUniformMat4("projection", projection);
	draw(chunk->getMesh());
}
