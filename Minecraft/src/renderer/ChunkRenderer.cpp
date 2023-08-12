#include "ChunkRenderer.h"

ChunkRenderer::ChunkRenderer(Shader shader, Camera& camera, GLuint texture_array)
	: Renderer(shader),
	  m_texture_array{texture_array},
      m_camera{camera}
	  
{
	m_vertexpool = new VertexPool{ };
	m_terrain_generator = new TerrainGenerator{ };
	m_total_faces_added = 0;
	m_buffer_needs_update = true;
}

void ChunkRenderer::render(Camera& camera)
{

}

void ChunkRenderer::drawChunksSceneMesh()
{
	m_shader.bind();
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = m_camera.getViewMatrix();
	glm::mat4 projection = m_camera.getProjectionMatrix();
	m_shader.setUniformMat4("model", model);
	m_shader.setUniformMat4("view", view);
	m_shader.setUniformMat4("projection", projection);

	m_vertexpool->draw(m_world_mesh_daic.size());
}

void ChunkRenderer::createInRenderDistanceAndDestroyOutOfRenderDistanceChunks()
{
	destroyOutOfRenderDistanceChunks();
	createInRenderDistanceChunks();
	if (m_buffer_needs_update) {
		m_vertexpool->updateBuffer(m_world_mesh, m_world_mesh_daic);
		m_buffer_needs_update = false;
	}
}

void ChunkRenderer::traverseScene()
{
	OPTICK_EVENT("traverseScene");
	int camera_chunk_pos_x = m_camera.getCameraPos().x / CHUNK_SIZE;
	int min_x = camera_chunk_pos_x - (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2);
	int max_x = camera_chunk_pos_x + (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2);

	int camera_chunk_pos_z = m_camera.getCameraPos().z / CHUNK_SIZE;
	int min_z = camera_chunk_pos_z - (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2);
	int max_z = camera_chunk_pos_z + (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2);

	for (int cx = max_x; cx > min_x; cx--)
	{
		for (int cz = max_z; cz > min_z; cz--)
		{
			for (int cy = ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1; cy >= 0; cy--)
			{
				createChunkIfNotPresent({cx, cy, cz});
			}
		}
	}

	for (Chunk& chunk : m_all_chunks)
	{
		int cx = chunk.getPos().x;
		int cy = chunk.getPos().y;
		int cz = chunk.getPos().z;

		if (
			cx < min_x ||
			cx > max_x ||
			cz < min_z ||
			cz > max_z
			)
		{
			m_chunks_to_delete.push({ cx, cy, cz });
		}
	}

	if (m_buffer_needs_update) {
		m_vertexpool->updateBuffer(m_world_mesh, m_world_mesh_daic);
		m_buffer_needs_update = false;
	}
}

void ChunkRenderer::createInRenderDistanceChunks()
{
	while (!m_chunks_to_create.empty())
	{
		glm::ivec3 chunk_pos = m_chunks_to_create.front();
		createChunkIfNotPresent(chunk_pos);
		m_chunks_to_create.pop();
	}
}

void ChunkRenderer::destroyOutOfRenderDistanceChunks()
{
}

void ChunkRenderer::createChunkIfNotPresent(glm::ivec3 chunk_pos)
{
	OPTICK_EVENT("createChunkIfNotPresent");
	if (m_chunks_by_coord.find(chunk_pos) != m_chunks_by_coord.end())
		return;

	m_buffer_needs_update = true;
	createChunk(chunk_pos);
}

void ChunkRenderer::createChunk(glm::ivec3 chunk_pos)
{
	OPTICK_EVENT("createChunk");
	LevelOfDetail::LevelOfDetail lod = LevelOfDetail::chooseLevelOfDetail(m_camera, chunk_pos);
	std::unique_ptr<Chunk> chunk{ new Chunk(chunk_pos, lod) };
	m_terrain_generator->generateChunkTerrain(*chunk);

	// TODO: This needs to be done better, do after assembling working pipeline
	chunk->addChunkMesh();
	std::vector<Vertex> chunk_mesh{ chunk->getMesh().getMeshDataCopy() };
	m_world_mesh.insert(m_world_mesh.end(), chunk_mesh.begin(), chunk_mesh.end());
	//
	unsigned int added_faces = chunk->getAddedFacesAmount();
	m_total_faces_added += added_faces;
	
	// TODO: mutex
	unsigned int draw_commands_added = m_world_mesh_daic.size() - 1;
	DAIC daic
	{
		6 * added_faces, // vertices in face * added_faces
		1,
		6 * m_total_faces_added , // command offset in the buffer
		0
	};
	m_world_mesh_daic.push_back(daic);
	// TODO: mutex
	m_all_chunks.emplace_back(*chunk);
	// TODO: mutex
	m_chunks_by_coord[chunk_pos] = *chunk;
}

void ChunkRenderer::deleteChunk(glm::ivec3 chunk_pos)
{
}

void ChunkRenderer::processChunks()
{

}

void ChunkRenderer::processChunk(Chunk& chunk)
{
}
