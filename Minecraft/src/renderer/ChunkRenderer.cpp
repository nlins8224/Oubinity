#include "ChunkRenderer.h"

ChunkRenderer::ChunkRenderer(Shader shader, Camera& camera, GLuint texture_array)
	: Renderer(shader),
	  m_texture_array{texture_array},
      m_camera{camera}
	  
{
	m_vertexpool = new VertexPool::ZoneVertexPool{ };
	m_terrain_generator = new TerrainGenerator{ };
	initChunks();
	m_buffer_needs_update = true;
	m_camera_last_chunk_pos = { -999, -999, -999 };
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
		traverseScene();
	}
}

void ChunkRenderer::initChunks()
{
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

	for (int cx = max_x; cx > min_x; cx--)
	{
		for (int cz = max_z; cz > min_z; cz--)
		{
			for (int cy = ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1; cy >= 0; cy--)
			{
				if (!m_chunks_by_coord.if_contains({cx, cy, cz}, [](auto) {}))
				{
					m_chunks_to_create.push({ cx, cy, cz });
				}
			}
		}
	}

	createInRenderDistanceChunks();
	decorateChunks();
	meshChunks();
	updateBufferIfNeedsUpdate();
}

// render thread

// TODO: traveseScene can be slower than camera's moevement, which causes 
// some updates to be lost. All actions should be queued or traverseScene should be faster
void ChunkRenderer::traverseScene()
{
	OPTICK_EVENT("traverseScene");

	int camera_chunk_pos_x = m_camera.getCameraPos().x / CHUNK_SIZE;
	int camera_chunk_pos_z = m_camera.getCameraPos().z / CHUNK_SIZE;

	int dx = std::abs(camera_chunk_pos_x - m_camera_last_chunk_pos.x);
	int dz = std::abs(camera_chunk_pos_z - m_camera_last_chunk_pos.z);
	if (dx < 1 && dz < 1)
	{
		return;
	}
	if (dx > 1 || dz > 1)
	{
		LOG_F(WARNING, "Delta between last and current camera pos dx: %d, dz: %d, but should be <= 1", dx, dz);
	}

	m_camera_last_chunk_pos.x = camera_chunk_pos_x;
	m_camera_last_chunk_pos.z = camera_chunk_pos_z;

	ChunkBorder chunk_border;
	for (int i = 1; i < LevelOfDetail::Lods.size(); i++)
	{
		int border_dist = LevelOfDetail::Lods[i].draw_distance / 2;
		chunk_border.min_x = camera_chunk_pos_x - border_dist;
		chunk_border.max_x = camera_chunk_pos_x + border_dist;
		chunk_border.min_z = camera_chunk_pos_z - border_dist;
		chunk_border.max_z = camera_chunk_pos_z + border_dist;
		iterateOverChunkBorderAndUpdateLod(chunk_border);
	}

	chunk_border.min_x = camera_chunk_pos_x - (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2);
	chunk_border.max_x = camera_chunk_pos_x + (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2);
	chunk_border.min_z = camera_chunk_pos_z - (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2);
	chunk_border.max_z = camera_chunk_pos_z + (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2);

	iterateOverChunkBorderAndDelete(chunk_border);
	iterateOverChunkBorderAndCreate(chunk_border);

	if (m_chunks_to_create.size() > 0 || m_chunks_to_delete.size() > 0)
	{
		m_buffer_needs_update.store(m_buffer_needs_update | deleteOutOfRenderDistanceChunks() | createInRenderDistanceChunks() | decorateChunks() | meshChunks());
	}
}

void ChunkRenderer::iterateOverChunkBorderAndCreate(ChunkBorder chunk_border)
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
			}

			if (!m_chunks_by_coord.if_contains({ max_x, cy, cz }, [](auto) {}))
			{
				m_chunks_to_create.push({ max_x, cy, cz });
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
			}

			if (!m_chunks_by_coord.if_contains({ cx, cy, max_z }, [](auto) {}))
			{
				m_chunks_to_create.push({ cx, cy, max_z });
			}
		}
	}
}

void ChunkRenderer::iterateOverChunkBorderAndDelete(ChunkBorder chunk_border)
{
	int min_x = chunk_border.min_x;
	int max_x = chunk_border.max_x;
	int min_z = chunk_border.min_z;
	int max_z = chunk_border.max_z;

	// x-/x+ iterate over z
	for (int cz = min_z - 1; cz < max_z + 1; cz++)
	{
		for (int cy = ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1; cy >= 0; cy--)
		{
			if (m_chunks_by_coord.if_contains({ min_x - 1, cy, cz }, [](auto) {}))
			{
				m_chunks_to_delete.push({ min_x - 1, cy, cz });
			}

			if (m_chunks_by_coord.if_contains({ max_x + 1, cy, cz }, [](auto) {}))
			{
				m_chunks_to_delete.push({ max_x + 1, cy, cz });
			}
		}
	}

	// z-/z+ iterate over x
	for (int cx = min_x - 1; cx < max_x + 1; cx++)
	{
		for (int cy = ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1; cy >= 0; cy--)
		{
			if (m_chunks_by_coord.if_contains({ cx, cy, min_z - 1 }, [](auto) {}))
			{
				m_chunks_to_delete.push({ cx, cy, min_z - 1 });
			}

			if (m_chunks_by_coord.if_contains({ cx, cy, max_z + 1 }, [](auto) {}))
			{
				m_chunks_to_delete.push({ cx, cy, max_z + 1 });
			}
		}
	}
}

void ChunkRenderer::iterateOverChunkBorderAndUpdateLod(ChunkBorder chunk_border)
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
			if (checkIfChunkLodNeedsUpdate({ min_x, cy, cz }))
			{
				m_chunks_to_delete.push({ min_x, cy, cz });
				m_chunks_to_create.push({ min_x, cy, cz });
			}

			if (checkIfChunkLodNeedsUpdate({ max_x, cy, cz }))
			{
				m_chunks_to_delete.push({ max_x, cy, cz });
				m_chunks_to_create.push({ max_x, cy, cz });
			}
		}
	}

	// z-/z+ iterate over x
	for (int cx = min_x; cx < max_x; cx++)
	{
		for (int cy = ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1; cy >= 0; cy--)
		{
			if (checkIfChunkLodNeedsUpdate({ cx, cy, min_z }))
			{
				m_chunks_to_delete.push({ cx, cy, min_z });
				m_chunks_to_create.push({ cx, cy, min_z });
			}

			if (checkIfChunkLodNeedsUpdate({ cx, cy, max_z }))
			{
				m_chunks_to_delete.push({ cx, cy, max_z });
				m_chunks_to_create.push({ cx, cy, max_z });
			}
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
	if (m_buffer_needs_update.load()) {
			// free should go first, before allocate
			freeChunks();
			allocateChunks();
			m_vertexpool->createChunkInfoBuffer();
			m_vertexpool->createChunkLodBuffer();
			m_buffer_needs_update.store(false);
		}	
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

	glm::ivec3 camera_pos = m_camera.getCameraPos() / static_cast<float>(CHUNK_SIZE);
	LevelOfDetail::LevelOfDetail lod = LevelOfDetail::chooseLevelOfDetail(camera_pos, chunk_pos);
	HeightMap height_map = m_terrain_generator->generateHeightMap(chunk_pos, lod);
	bool is_chunk_visible = !m_terrain_generator->isChunkBelowOrAboveSurface(chunk_pos, height_map, lod);
	if (!is_chunk_visible) {
		return;
	}

	m_chunks_by_coord.lazy_emplace_l(chunk_pos,
		[](auto) {}, // unused, called if value is already present, we know that it is not
		[&](const pmap::constructor& ctor) {
			Chunk* chunk = new Chunk(chunk_pos, lod);
			m_terrain_generator->generateChunkTerrain(*chunk, height_map, is_chunk_visible);
			ctor(chunk_pos, std::move(chunk));
			m_chunks_to_decorate.push(chunk_pos);
			chunk->setState(ChunkState::CREATED);
		});
}

// render thread
bool ChunkRenderer::decorateChunkIfPresent(glm::ivec3 chunk_pos)
{
	ChunkNeighbors chunk_neighbors;
	int x = chunk_pos.x, y = chunk_pos.y, z = chunk_pos.z;

	for (int x_offset : {-1, 0, 1}) {
		for (int y_offset : {-1, 0, 1}) {
			for (int z_offset : {-1, 0, 1}) {
				glm::ivec3 target_chunk_pos = { x + x_offset, y + y_offset, z + z_offset };
				m_chunks_by_coord.if_contains(target_chunk_pos, [&](const pmap::value_type& pair) { chunk_neighbors[target_chunk_pos] = pair.second; });
			}
		}
	}

	m_chunks_by_coord.modify_if(chunk_pos,
		[&](const pmap::value_type& pair) {
			pair.second->setNeighbors(chunk_neighbors);
			#if SETTING_TREES_ENABLED
				m_terrain_generator->generateTrees(*pair.second);
			#endif
			m_chunks_to_mesh.push(chunk_pos);
			pair.second->setState(ChunkState::DECORATED);
		});

	return true;
}

// render thread
bool ChunkRenderer::decorateChunks()
{
	bool anything_decorated = false;
	while (!m_chunks_to_decorate.empty())
	{
		glm::ivec3 chunk_pos = m_chunks_to_decorate.front();
		anything_decorated |= decorateChunkIfPresent(chunk_pos);
		m_chunks_to_decorate.pop();
	}
	return anything_decorated;
}

// render thread
bool ChunkRenderer::meshChunks()
{
	LOG_F(INFO, "meshChunks called");
	LOG_F(INFO, "chunks to mesh: %d", m_chunks_to_mesh.size());
	bool anything_meshed = false;
	while (!m_chunks_to_mesh.empty())
	{
		glm::ivec3 chunk_pos = m_chunks_to_mesh.front();
		anything_meshed |= meshChunk(chunk_pos);
		m_chunks_to_mesh.pop();
		m_chunks_to_allocate.push(chunk_pos);
	}
	return anything_meshed;
}

// render thread
bool ChunkRenderer::meshChunk(glm::ivec3 chunk_pos)
{
	m_chunks_by_coord.modify_if(chunk_pos,
		[&](const pmap::value_type& pair) {
			if (pair.second->isVisible()) {
				pair.second->addChunkMesh();
			}
			pair.second->setState(ChunkState::MESHED);
		});

	return true;
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
	glm::ivec3 camera_pos = m_camera.getCameraPos() / static_cast<float>(CHUNK_SIZE);
	LevelOfDetail::LevelOfDetail lod = LevelOfDetail::chooseLevelOfDetail(camera_pos, chunk_pos);
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
		allocateChunk();
	}
}

// main thread
void ChunkRenderer::allocateChunk()
{
	glm::ivec3 chunk_pos = m_chunks_to_allocate.front();
	VertexPool::ChunkAllocData alloc_data;
	m_chunks_by_coord.modify_if(chunk_pos,
		[&](const pmap::value_type& pair) {
			alloc_data._chunk_pos = pair.second->getPos();
			alloc_data._added_faces_amount = pair.second->getAddedFacesAmount();
			alloc_data._lod = pair.second->getLevelOfDetail();
			alloc_data._mesh = std::move(pair.second->getMesh().getMeshData()); // chunk mesh is about to be allocated, vertex pool takes ownership
			alloc_data._mesh_faces = std::move(pair.second->getFaces());
			alloc_data._chunk_world_pos = pair.second->getWorldPos();
			alloc_data._ready = true;
			pair.second->setState(ChunkState::ALLOCATED);
		});
	if (alloc_data._ready)
	{
		m_vertexpool->allocate(std::move(alloc_data));
	}
	m_chunks_to_allocate.pop();
}

// main thread
void ChunkRenderer::freeChunks()
{
	while (!m_chunks_to_free.empty())
	{
		freeChunk();
	}
}

// main thread
void ChunkRenderer::freeChunk()
{
	glm::ivec3 chunk_pos = m_chunks_to_free.front();
	m_vertexpool->free(chunk_pos);
	m_chunks_to_free.pop();
}
