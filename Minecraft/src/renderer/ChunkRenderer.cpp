#include "ChunkRenderer.h"

ChunkRenderer::ChunkRenderer(Shader shader)
	: Renderer(shader)
{
	m_chunks_map = nullptr;
}

ChunkRenderer::~ChunkRenderer()
{
	delete m_chunks_map;
}

void ChunkRenderer::render(Camera& camera)
{
	if (m_chunks_map->empty())
		return;

	m_shader.bind();
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = camera.getProjectionMatrix();
	m_shader.setUniformMat4("model", model);
	m_shader.setUniformMat4("view", view);
	m_shader.setUniformMat4("projection", projection);

	for (auto& [_, chunk] : *m_chunks_map)
	{
		renderChunk(camera, chunk);
	}
}

void ChunkRenderer::setChunks(ChunksMap* chunks_map)
{
	m_chunks_map = chunks_map;
}

void ChunkRenderer::draw(Mesh& mesh) const
{
	mesh.getLoader().bindVAO();
	uint8_t vertices_per_triangle{ 3 };
	uint64_t amount_of_triangles{ mesh.getMeshVertexPositions().size() / vertices_per_triangle};
	glDrawArrays(GL_TRIANGLES, 0, amount_of_triangles);
}

void ChunkRenderer::renderChunk(Camera& camera, Chunk& chunk)
{
	chunk.prepareChunkMesh();
	draw(chunk.getMesh());
}
