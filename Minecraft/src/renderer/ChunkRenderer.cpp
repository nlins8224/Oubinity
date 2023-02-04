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

void ChunkRenderer::draw(const Mesh& mesh) const
{
	mesh.getLoader().bindVAO();
	glDrawArrays(GL_TRIANGLES, 0, mesh.getTrianglesCount());
}

void ChunkRenderer::renderChunk(Camera& camera, Chunk& chunk) const
{
	m_shader.setUniformVec3f("chunk_world_pos", chunk.getWorldPos());

	if (chunk.getMesh().getMeshState() == MeshState::READY)
	{
		chunk.addChunkMesh();
		chunk.getMesh().setMeshState(MeshState::PROCESSED);
	}

	if (chunk.getMesh().getMeshState() == MeshState::PROCESSED)
	{
		chunk.getMesh().loadPackedMesh();
		chunk.getMesh().setMeshState(MeshState::LOADED);
	}

	
	AABox box{ chunk.getWorldPos(), glm::vec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z)};
	bool chunk_in_frustum = camera.getFrustum().isBoxInFrustum(box);
	if (!chunk_in_frustum)
		return;

	draw(chunk.getMesh());
}
