#include "ChunkRenderer.h"

ChunkRenderer::ChunkRenderer(Shader shader, Camera& camera, GLuint texture_array)
	: Renderer(shader),
	  m_texture_array{texture_array},
      m_camera{camera}
	  
{
	m_vertexpool = new VertexPool{ };
	m_terrain_generator = new TerrainGenerator{ };
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

	m_vertexpool->draw();
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
				cz > max_z ||
				checkIfChunkLodNeedsUpdate({ cx, cy, cz })
				)
			{
				m_chunks_to_delete.push({ cx, cy, cz });
			}
		}
		m_buffer_needs_update.store(m_buffer_needs_update | deleteOutOfRenderDistanceChunks() | createInRenderDistanceChunks());
}

void ChunkRenderer::updateBufferIfNeedsUpdate()
{
	OPTICK_EVENT("updateBufferIfNeedsUpdate");
	if (m_buffer_needs_update.load()) {
		m_vertexpool->createChunkInfoBuffer();
		m_vertexpool->createChunkLodBuffer();
		m_buffer_needs_update.store(false);
	}
}

void ChunkRenderer::runTraverseSceneInDetachedThread()
{
	std::thread(&ChunkRenderer::traverseScene, this).detach();
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
	createChunkTask(*chunk);
	
	m_chunks_by_coord[chunk_pos] = *chunk;
}

void ChunkRenderer::createChunkTask(Chunk& chunk)
{
	m_terrain_generator->generateChunkTerrain(chunk);
	chunk.addChunkMesh();
	m_vertexpool->allocate(chunk);
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

void ChunkRenderer::deleteChunk(glm::ivec3 chunk_pos)
{
	m_chunks_by_coord.erase(chunk_pos);
	m_vertexpool->free(chunk_pos);
}

bool ChunkRenderer::checkIfChunkLodNeedsUpdate(glm::ivec3 chunk_pos)
{
	LevelOfDetail::LevelOfDetail lod = LevelOfDetail::chooseLevelOfDetail(m_camera, chunk_pos);
	return m_chunks_by_coord[chunk_pos].getLevelOfDetail().level != lod.level;
}

