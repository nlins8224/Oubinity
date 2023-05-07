#include "ChunkRenderer.h"


ChunkRenderer::ChunkRenderer(Shader shader, ChunksMap& chunks_map, std::atomic<bool>& is_ready_to_process_chunks, GLuint texture_array)
	: Renderer(shader),
	  m_chunks_map(chunks_map),
	  m_is_ready_to_process_chunks{is_ready_to_process_chunks},
	  m_texture_array{texture_array}
	  
{
}

ChunkRenderer::~ChunkRenderer()
{
}

void ChunkRenderer::launchChunkProcessingTask()
{
	std::thread(&ChunkRenderer::processChunksMeshTask, this).detach();
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
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array);

	glDrawArrays(GL_TRIANGLES, 0, mesh.getTrianglesCount());
	mesh.getLoader().unbindVAO();
}

void ChunkRenderer::processChunkMesh(Chunk& chunk) const
{
	OPTICK_EVENT();
	if (chunk.getMesh().getMeshState() == MeshState::READY)
	{	
		chunk.addChunkMesh();
		chunk.getMesh().setMeshState(MeshState::READY_TO_DECORATE);
		//std::cout << "READY_TO_DECORATE!" << std::endl;
	}	
	if (chunk.getMesh().getMeshState() == MeshState::DECORATED)
	{
		chunk.addChunkDecorationMesh();
		chunk.getMesh().setMeshState(MeshState::PROCESSED);
		//std::cout << "PROCESSED!" << std::endl;
	}
}

void ChunkRenderer::processChunksMeshTask() const
{
	while (true)
	{
		m_is_ready_to_process_chunks.wait(false);

		for (auto& [_, chunk] : m_chunks_map)
		{
			processChunkMesh(chunk);
		}

		m_is_ready_to_process_chunks.store(false);
		m_is_ready_to_process_chunks.notify_one();
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
	if (chunk.getMesh().getMeshState() != MeshState::LOADED)
		return;

	if (!isInFrustum(camera, chunk))
		return;

	draw(chunk.getMesh());
}
