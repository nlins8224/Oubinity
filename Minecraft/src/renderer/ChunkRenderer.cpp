#include "ChunkRenderer.h"
#include <iostream>
ChunkRenderer::ChunkRenderer(Shader shader, ChunksMap& chunks_map, std::atomic<bool>& is_ready_to_process_chunks, GLuint texture_array)
	: Renderer(shader),
	  m_chunks_map(chunks_map),
	  m_is_ready_to_process_chunks{is_ready_to_process_chunks},
	  m_texture_array{texture_array}
	  
{
	m_daic_offset_counter = 0;
	m_vertexpool = new VertexPool{ };
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

	loadWorldMesh();
	drawWorldMesh();
}

void ChunkRenderer::loadWorldMesh()
{
	//std::cout << "Mesh Data Size: " << m_world_mesh.getMeshData().size() << std::endl;
	//std::cout << "DAIC amount: " << m_world_mesh_daic.size() << std::endl;
	if (!buffer_loaded && m_world_mesh_daic.size() >= 149)
	{
		std::vector<Vertex> mesh_data = m_world_mesh.getMeshDataCopy();
		std::cout << "Mesh size: " << mesh_data.size() << std::endl;
		std::cout << "Daic size: " << m_world_mesh_daic.size() << std::endl;
		m_vertexpool->updateBuffer(mesh_data, m_world_mesh_daic);
		buffer_loaded = true;
	}
	
	//m_daic_offset_counter = 0;
	//m_world_mesh_test.clear();
	//m_world_mesh_daic.clear();
}

void ChunkRenderer::drawWorldMesh()
{
	//std::cout << "Triangles amount: " << m_world_mesh.getMeshData().size() / 3 << std::endl;
	m_vertexpool->draw(m_world_mesh_daic.size());
}

void ChunkRenderer::processChunkMesh(Chunk& chunk)
{
	if (chunk.getMesh().getMeshState() == MeshState::READY)
	{	
		chunk.addChunkMesh();
		chunk.getMesh().setMeshState(MeshState::MESHED);
	}	
	if (chunk.getMesh().getMeshState() == MeshState::MESHED)
	{
		//chunk.addChunkDecorationMesh();
		chunk.getMesh().setMeshState(MeshState::PROCESSED);
		
		std::vector<Vertex> mesh_data = chunk.getMesh().getMeshDataCopy();

		if (mesh_data.size() == 0)
			return;

		m_world_mesh.addMesh(mesh_data);
		//std::cout << "Chunk mesh size: " << chunk.getMesh().getMeshData().size() << std::endl;
		//std::cout << "Added faces amount: " << chunk.getAddedFacesAmount() << std::endl;
		//std::cout << "World mesh size: " << m_world_mesh.getMeshDataCopy().size() << std::endl;
		//std::cout << "DAIC offset counter: " << m_daic_offset_counter << std::endl;
		unsigned int faces_added = chunk.getAddedFacesAmount();
		DAIC daic {
			6 * faces_added, // 6 vertices per face
			1, 
			m_daic_offset_counter * 6 * faces_added,
			0
		};
		m_world_mesh_daic.emplace_back(daic);
		m_daic_offset_counter++;
		std::cout << m_daic_offset_counter << std::endl;
	}
}

void ChunkRenderer::processChunksMeshTask()
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
