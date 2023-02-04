#include "ChunkRenderer.h"

ChunkRenderer::ChunkRenderer(Shader shader, ChunksMap& chunks_map)
	: Renderer(shader),
	  m_chunks_map(chunks_map)
{
}

ChunkRenderer::~ChunkRenderer()
{
}

void ChunkRenderer::render(Camera& camera)
{
	if (m_chunks_map.empty())
		return;

	m_shader.bind();
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = camera.getProjectionMatrix();
	m_shader.setUniformMat4("model", model);
	m_shader.setUniformMat4("view", view);
	m_shader.setUniformMat4("projection", projection);

	processChunksMesh();

	for (auto& [_, chunk] : m_chunks_map)
	{
		loadChunkMesh(chunk);
	}

	for (auto& [_, chunk] : m_chunks_map)
	{
		renderChunk(camera, chunk);
	}
}

void ChunkRenderer::draw(const Mesh& mesh) const
{
	mesh.getLoader().bindVAO();
	glDrawArrays(GL_TRIANGLES, 0, mesh.getTrianglesCount());
}

void ChunkRenderer::processChunkMesh(Chunk& chunk) const
{
	if (chunk.getMesh().getMeshState() == MeshState::READY)
	{
		chunk.addChunkMesh();
		chunk.getMesh().setMeshState(MeshState::PROCESSED);
	}
	
}

void ChunkRenderer::processChunksMesh()
{
	if (m_chunks_map.empty())
		return;

	for (auto& [_, chunk] : m_chunks_map)
	{
		processChunkMesh(chunk);
	}
}

void ChunkRenderer::loadChunkMesh(Chunk& chunk) const
{
	if (chunk.getMesh().getMeshState() == MeshState::PROCESSED)
	{
		chunk.getMesh().loadPackedMesh();
		chunk.getMesh().setMeshState(MeshState::LOADED);
	}
}

bool ChunkRenderer::isInFrustum(Camera& camera, Chunk& chunk) const
{
	AABox box{ chunk.getWorldPos(), glm::vec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z) };
	return camera.getFrustum().isBoxInFrustum(box);
}

void ChunkRenderer::renderChunk(Camera& camera, Chunk& chunk) const
{
	m_shader.setUniformVec3f("chunk_world_pos", chunk.getWorldPos());
	
	if (!isInFrustum(camera, chunk))
		return;

	draw(chunk.getMesh());
}
