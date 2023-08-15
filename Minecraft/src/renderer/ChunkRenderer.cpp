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

	m_vertexpool->draw(m_active_daics.size());
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
				m_chunks_to_create.push({ cx, cy, cz });
			}
		}
	}

	for (auto& [chunk_pos, chunk] : m_chunks_by_coord)
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

	m_buffer_needs_update |= deleteOutOfRenderDistanceChunks();
	m_buffer_needs_update = createInRenderDistanceChunks();

	if (m_buffer_needs_update) {
		m_active_daics.clear();
		collectChunkShaderMetadata();
		m_vertexpool->updateDrawBuffer(m_all_chunks_mesh, m_active_daics);
		m_vertexpool->createChunkInfoBuffer(m_active_chunks_info);
		m_vertexpool->createChunkLodBuffer(m_active_chunks_lod);
		std::cout << "DRAW Commands: " << m_active_daics.size() << std::endl;
		m_buffer_needs_update = false;
	}
}

bool ChunkRenderer::createInRenderDistanceChunks()
{
	bool anything_created = false;
	while (!m_chunks_to_create.empty())
	{
		glm::ivec3 chunk_pos = m_chunks_to_create.front();
		anything_created |= createChunkIfNotPresent(chunk_pos);
		m_chunks_to_create.pop();
	}
	return anything_created;
}

bool ChunkRenderer::createChunkIfNotPresent(glm::ivec3 chunk_pos)
{
	OPTICK_EVENT("createChunkIfNotPresent");
	if (m_chunks_by_coord.find(chunk_pos) != m_chunks_by_coord.end())
		return false;

	createChunk(chunk_pos);
	return true;
}

void ChunkRenderer::createChunk(glm::ivec3 chunk_pos)
{
	OPTICK_EVENT("createChunk");
	LevelOfDetail::LevelOfDetail lod = LevelOfDetail::chooseLevelOfDetail(m_camera, chunk_pos);
	std::unique_ptr<Chunk> chunk{ new Chunk(chunk_pos, lod) };
	m_terrain_generator->generateChunkTerrain(*chunk);

	// TODO: This needs to be done better, do after assembling working pipeline
	chunk->addChunkMesh();
	unsigned int added_faces = chunk->getAddedFacesAmount();

	std::vector<Vertex> chunk_mesh{ chunk->getMesh().getMeshDataCopy() };
	m_all_chunks_mesh.insert(m_all_chunks_mesh.end(), chunk_mesh.begin(), chunk_mesh.end());
	//

	m_chunks_by_coord[chunk_pos] = *chunk;

	// There is no need to add chunk draw command if chunk is empty
	if (added_faces == 0)
	{
		return;
	}
	
	// TODO: mutex
	DAIC daic
	{
		6 * added_faces, // vertices in face * added_faces
		1,
		6 * m_total_faces_added , // vertices in face * command offset in the buffer
		0
	};
	m_total_faces_added += added_faces;
	
	ChunkShaderMetadata chunk_shader_metadata{daic, chunk->getWorldPos(), static_cast<GLuint>(lod.block_size)};
	m_chunks_shader_metadata[chunk_pos] = chunk_shader_metadata;

}

bool ChunkRenderer::deleteOutOfRenderDistanceChunks()
{
	bool anything_deleted = false;
	while (!m_chunks_to_delete.empty())
	{
		glm::ivec3 chunk_pos = m_chunks_to_delete.front();
		anything_deleted |= deleteChunkIfPresent(chunk_pos);
		m_chunks_to_delete.pop();
	}
	return anything_deleted;
}

bool ChunkRenderer::deleteChunkIfPresent(glm::ivec3 chunk_pos)
{
	if (m_chunks_by_coord.find(chunk_pos) == m_chunks_by_coord.end())
		return false;

	deleteChunk(chunk_pos);
	return true;
}

/*
deleteChunk
steps to do:
1. delete chunk from m_chunks_shader_metadata
2. delete chunk from m_chunks_by_coord 
*/
void ChunkRenderer::deleteChunk(glm::ivec3 chunk_pos)
{
	if (m_chunks_shader_metadata.find(chunk_pos) != m_chunks_shader_metadata.end())
		m_chunks_shader_metadata.erase(chunk_pos);
	m_chunks_by_coord.erase(chunk_pos);
}

void ChunkRenderer::collectChunkShaderMetadata()
{
	int index = 0;
	for (auto& [chunk_pos, chunk_metadata] : m_chunks_shader_metadata)
	{
		m_active_daics.push_back(chunk_metadata._daic);
		m_active_chunks_info.chunk_pos[index] = { chunk_metadata._chunk_world_pos, index };
		m_active_chunks_lod.chunks_lod[index] = chunk_metadata._lod;
		index++;
	}
}
