#include "ChunkRenderer.h"

ChunkRenderer::ChunkRenderer(Shader shader, Camera& camera, GLuint texture_array)
	: Renderer(shader),
	  m_texture_array{texture_array},
      m_camera{camera}
	  
{
	m_vertexpool = new VertexPool::ZoneVertexPool{ };
	m_terrain_generator = new TerrainGenerator{ };
	m_buffer_needs_update = true;
}

void ChunkRenderer::render(Camera& camera)
{

}

// main thread
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

// render thread
void ChunkRenderer::traverseSceneLoop()
{
	while (true)
	{
		//m_cond_atomic_flag.wait(true);
		traverseScene();
		//m_cond_atomic_flag.store(true);
		//m_cond_atomic_flag.notify_one();
	}
}

// TODO: This scans entire scene, is a bottleneck and is very naive in it's approach
// What should be done instead is to traverse only through "bordering" chunks, that is, 
// chunks that are sitting next to chunks with different LOD or next to empty space
// OR/AND use flat array instead of hashmap and rely on position on the grid and camera motion
// instead of coords

// render thread
void ChunkRenderer::traverseScene()
{
	OPTICK_EVENT("traverseScene");

	int camera_chunk_pos_x = m_camera.getCameraPos().x / CHUNK_SIZE;
	int min_x = camera_chunk_pos_x - (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2);
	int max_x = camera_chunk_pos_x + (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2);

	int camera_chunk_pos_z = m_camera.getCameraPos().z / CHUNK_SIZE;
	int min_z = camera_chunk_pos_z - (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2);
	int max_z = camera_chunk_pos_z + (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2);

	ChunkBorder chunk_border;
	chunk_border.min_x = min_x;
	chunk_border.max_x = max_x;
	chunk_border.min_z = min_z;
	chunk_border.max_z = max_z;

	iterateOverBorderChunksAndDelete(chunk_border);
	iterateOverBorder(chunk_border);

	//for (int cx = max_x; cx > min_x; cx--)
	//{
	//	for (int cz = max_z; cz > min_z; cz--)
	//	{
	//		for (int cy = ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1; cy >= 0; cy--)
	//		{
	//			if (!m_chunks_by_coord.if_contains({cx, cy, cz}, [](auto) {}))
	//			{
	//				m_chunks_to_create.push({ cx, cy, cz });
	//			}
	//		}
	//	}
	//}

	// ITERATING OVER THE MAP IS UNSAFE
	//for (auto& [chunk_pos, chunk] : m_chunks_by_coord)
	//{
	//	int cx = chunk->getPos().x;
	//	int cy = chunk->getPos().y;
	//	int cz = chunk->getPos().z;

	//	if (
	//		cx < min_x ||
	//		cx > max_x ||
	//		cz < min_z ||
	//		cz > max_z ||
	//		checkIfChunkLodNeedsUpdate({ cx, cy, cz })
	//		)
	//	{
	//		m_chunks_to_delete.push({ cx, cy, cz });
	//	}
	//}

	if (m_chunks_to_create.size() > 0 || m_chunks_to_delete.size() > 0)
	{
		m_buffer_needs_update.store(m_buffer_needs_update | deleteOutOfRenderDistanceChunks() | createInRenderDistanceChunks());
	}
}

void ChunkRenderer::iterateOverBorder(ChunkBorder chunk_border)
{	
	int min_x = chunk_border.min_x;
	int max_x = chunk_border.max_x;
	int min_z = chunk_border.min_z;
	int max_z = chunk_border.max_z;

	// x-/x+ iterate over z
	for (int cz = min_z; cz < max_z; cz++)
	{
		for (int cy = ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1; cy >= 0; cy--)
		{
			if (!m_chunks_by_coord.if_contains({ min_x, cy, cz }, [](auto) {}))
			{
				m_chunks_to_create.push({ min_x, cy, cz });
				m_border_chunks.push_back({ min_x, cy, cz });
			}

			if (!m_chunks_by_coord.if_contains({ max_x, cy, cz }, [](auto) {}))
			{
				m_chunks_to_create.push({ max_x, cy, cz });
				m_border_chunks.push_back({ max_x, cy, cz });

			}
		}
	}

	// z-/z+ iterate over x
	for (int cx = min_x; cx < max_x; cx++)
	{
		for (int cy = ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1; cy >= 0; cy--)
		{
			if (!m_chunks_by_coord.if_contains({ cx, cy, min_z }, [](auto) {}))
			{
				m_chunks_to_create.push({ cx, cy, min_z });
				m_border_chunks.push_back({ cx, cy, min_z });
			}

			if (!m_chunks_by_coord.if_contains({ cx, cy, max_z }, [](auto) {}))
			{
				m_chunks_to_create.push({ cx, cy, max_z });
				m_border_chunks.push_back({ cx, cy, max_z });
			}
		}
	}
}

void ChunkRenderer::iterateOverBorderChunksAndDelete(ChunkBorder chunk_border)
{
	std::vector<glm::ivec3>::iterator iter;
	for (iter = m_border_chunks.begin(); iter != m_border_chunks.end();)
	{
		glm::ivec3 chunk_pos = *iter;
		if (isChunkOutOfBorder(chunk_pos, chunk_border))
		{
			m_chunks_to_delete.push(chunk_pos);
			iter = m_border_chunks.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

bool ChunkRenderer::isChunkOutOfBorder(glm::ivec3 chunk_pos, ChunkBorder chunk_border)
{
	int cx = chunk_pos.x;
	int cz = chunk_pos.z;
		
	return cx < chunk_border.min_x || cx > chunk_border.max_x || cz < chunk_border.min_z || cz > chunk_border.max_z;
}

// main thread
void ChunkRenderer::updateBufferIfNeedsUpdate()
{
	OPTICK_EVENT("updateBufferIfNeedsUpdate");
	//while (true)
	//{
	//	m_cond_atomic_flag.wait(false);

		if (m_buffer_needs_update.load()) {
				// free should go first, before allocate
				freeChunks();
				allocateChunks();
				m_vertexpool->createChunkInfoBuffer();
				m_vertexpool->createChunkLodBuffer();
				m_buffer_needs_update.store(false);
			}

		//m_cond_atomic_flag.store(false);
		//m_cond_atomic_flag.notify_one();
	//}
	
}

void ChunkRenderer::runTraverseSceneInDetachedThread()
{
	std::thread(&ChunkRenderer::traverseSceneLoop, this).detach();
}
// render thread
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

// render thread
bool ChunkRenderer::createChunkIfNotPresent(glm::ivec3 chunk_pos)
{
	if (m_chunks_by_coord.if_contains(chunk_pos, [](auto) {}))
		return false;

	OPTICK_EVENT("createChunkIfNotPresent");
	createChunk(chunk_pos);
	return true;
}

// render thread
void ChunkRenderer::createChunk(glm::ivec3 chunk_pos)
{
	OPTICK_EVENT("createChunk");

	m_chunks_by_coord.lazy_emplace_l(chunk_pos,
		[](auto) {}, // unused, called if value is already present, we know that it is not
		[&](const pmap::constructor& ctor) {
			LevelOfDetail::LevelOfDetail lod = LevelOfDetail::chooseLevelOfDetail(m_camera, chunk_pos);
			Chunk* chunk = new Chunk(chunk_pos, lod);
			m_terrain_generator->generateChunkTerrain(*chunk);
			chunk->addChunkMesh();	
			ctor(chunk_pos, std::move(chunk));
			m_chunks_to_allocate.push(chunk_pos);
		});
}

// render thread
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

// render thread
bool ChunkRenderer::deleteChunkIfPresent(glm::ivec3 chunk_pos)
{
	if (!m_chunks_by_coord.if_contains(chunk_pos, [](auto) {}))
		return false;

	deleteChunk(chunk_pos);
	return true;
}

// render thread
void ChunkRenderer::deleteChunk(glm::ivec3 chunk_pos)
{
	m_chunks_to_free.push(chunk_pos);
	m_chunks_by_coord.erase_if(chunk_pos, [](auto) { return true; });
}

// render thread
bool ChunkRenderer::checkIfChunkLodNeedsUpdate(glm::ivec3 chunk_pos)
{
	LevelOfDetail::LevelOfDetail lod = LevelOfDetail::chooseLevelOfDetail(m_camera, chunk_pos);
	bool needs_update = false;
	m_chunks_by_coord.if_contains(chunk_pos, [&](const pmap::value_type& pair) {
		needs_update = pair.second->getLevelOfDetail().level != lod.level;
		});
	return needs_update;
}

// main thread
void ChunkRenderer::allocateChunks()
{
	while (!m_chunks_to_allocate.empty())
	{
		glm::ivec3 chunk_pos = m_chunks_to_allocate.front();
		VertexPool::ChunkAllocData alloc_data;
		m_chunks_by_coord.modify_if(chunk_pos,
			[&](const pmap::value_type& pair) {
				alloc_data._chunk_pos = pair.second->getPos();
				alloc_data._added_faces_amount = pair.second->getAddedFacesAmount();
				alloc_data._lod = pair.second->getLevelOfDetail();
				alloc_data._mesh = std::move(pair.second->getMesh().getMeshData()); // chunk mesh is about to be allocated, vertex pool takes ownership
				alloc_data._chunk_world_pos = pair.second->getWorldPos();
				alloc_data._ready = true;
			});
		if (alloc_data._ready)
		{
			m_vertexpool->allocate(std::move(alloc_data));
		}
		m_chunks_to_allocate.pop();
	}
}

// main thread
void ChunkRenderer::freeChunks()
{
	while (!m_chunks_to_free.empty())
	{
		glm::ivec3 chunk_pos = m_chunks_to_free.front();
		m_vertexpool->free(chunk_pos);
		m_chunks_to_free.pop();
	}
}
