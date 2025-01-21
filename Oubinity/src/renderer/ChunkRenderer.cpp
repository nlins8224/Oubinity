#include "ChunkRenderer.h"
#include "../Util.h"

ChunkRenderer::ChunkRenderer(TerrainGenerator& terrain_generator, Shader shader,
                             Camera& camera, GLuint texture_array)
    : m_terrain_generator{terrain_generator},
      Renderer(shader),
      m_texture_array{texture_array},
      m_camera{camera}

{
  m_vertexpool = new VertexPool::ZoneVertexPool{};
  m_init_stage = true;
  initChunks();
  m_buffer_needs_update = false;
  m_camera_last_chunk_pos = Util::worldPosToChunkPos(m_camera.getCameraPos());
}

void ChunkRenderer::render(Camera& camera) {}

// main thread
void ChunkRenderer::drawChunksSceneMesh() {
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
void ChunkRenderer::traverseSceneLoop() {
  while (true) {
    traverseScene();
    while (!m_tasks.empty()) {
      m_tasks.front()();
      m_tasks.pop();
    }
  }
}

std::weak_ptr<Chunk> ChunkRenderer::getChunkByWorldPos(glm::ivec3 world_block_pos) {
  glm::ivec3 chunk_pos = Util::worldPosToChunkPos(world_block_pos);
  return m_chunks_by_coord.get(chunk_pos);
}

block_id ChunkRenderer::getBlockIdByWorldPos(glm::ivec3 world_block_pos) {
  std::weak_ptr<Chunk> chunk = getChunkByWorldPos(world_block_pos);
  if (!chunk.lock()) {
    return block_id::NONE;
  }
  glm::ivec3 local_pos = Util::chunkWorldPosToLocalPos(world_block_pos);
  return chunk.lock()->getBlockId(local_pos);
}

bool ChunkRenderer::isBlockPresentByWorldPos(glm::ivec3 world_block_pos) {
  std::weak_ptr<Chunk> chunk = getChunkByWorldPos(world_block_pos);
  if (!chunk.lock()) {
    return block_id::NONE;
  }
  glm::ivec3 local_pos = Util::chunkWorldPosToLocalPos(world_block_pos);
  return chunk.lock()->isBlockPresent(local_pos);
}

// main thread
void ChunkRenderer::updateBlockByWorldPos(glm::ivec3 world_block_pos, block_id type) {
  glm::ivec3 chunk_pos = Util::worldPosToChunkPos(world_block_pos);
  LOG_F(INFO, "refreshChunk at chunk_pos=(%d,%d,%d)", chunk_pos.x, chunk_pos.y,
        chunk_pos.z);
  std::weak_ptr<Chunk> chunk = m_chunks_by_coord.get(chunk_pos);
  if (!chunk.lock()) {
    LOG_F(ERROR, "CHUNK DOES NOT EXIST (%d, %d, %d)", chunk_pos.x, chunk_pos.y,
          chunk_pos.z);
    return;
  }
  LOG_F(INFO, "was_chunk_edited=%d", chunk.lock()->wasChunkEdited());
  if (!chunk.lock()->wasChunkEdited()) {
    m_terrain_generator.generateChunkTerrain(*chunk.lock());
  }
  chunk.lock()->setBlock(Util::chunkWorldPosToLocalPos(world_block_pos), type);
  chunk.lock()->setWasChunkEdited(true);
  meshChunk(chunk_pos);
  freeChunk(chunk_pos);
  allocateChunk(chunk_pos);
  m_vertexpool->createChunkInfoBuffer();
  m_vertexpool->createChunkLodBuffer();
}

void ChunkRenderer::initChunks() {
  glm::ivec3 camera_chunk_pos =
      Util::worldPosToChunkPos(m_camera.getCameraPos());
  int camera_chunk_pos_x = camera_chunk_pos.x;
  int camera_chunk_pos_z = camera_chunk_pos.z;

  int border_dist =
      ((Settings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS) / 2);
  int min_x = camera_chunk_pos_x - border_dist;
  int max_x = camera_chunk_pos_x + border_dist - 1;
  int min_z = camera_chunk_pos_z - border_dist;
  int max_z = camera_chunk_pos_z + border_dist - 1;

  m_chunks_by_coord = ChunkSlidingWindow({min_x, max_x, min_z, max_z});
  LOG_F(INFO, "Chunk Border min_x=%d, max_x=%d, min_z=%d, max_z=%d",
        min_x, max_x, min_z, max_z);
  for (int cx = min_x; cx <= max_x; cx++) {
    for (int cz = min_z; cz <= max_z; cz++) {
      for (int cy = Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1;
           cy >= 0; cy--) {
        m_chunks_to_create.push({cx, cy, cz});
      }
    }
  }

  createChunksInRenderDistance();
  populateChunksNeighbors();
  generateChunksTerrain();
  decorateChunks();
  meshChunks();
  m_buffer_needs_update = true;
  updateBufferIfNeedsUpdate();
  m_init_stage = false;
}

// render thread

// TODO: traveseScene could be slower than camera's moevement, which causes
// some updates to be lost. All actions should be queued or traverseScene should
// be faster
void ChunkRenderer::traverseScene() {
  glm::ivec3 last_camera_chunk_pos =
      Util::worldPosToChunkPos(m_camera.getCameraPos());
  int last_camera_chunk_pos_x = last_camera_chunk_pos.x;
  int last_camera_chunk_pos_z = last_camera_chunk_pos.z;

  int dx = std::abs(last_camera_chunk_pos_x - m_camera_last_chunk_pos.x);
  int dz = std::abs(last_camera_chunk_pos_z - m_camera_last_chunk_pos.z);
  if (dx < 1 && dz < 1) {
    return;
  }
  if (dx > 1 || dz > 1) {
    LOG_F(WARNING,
          "Delta between last and current camera pos dx: %d, dz: %d, but "
          "should be <= 1",
          dx, dz);
  }
  LOG_F(INFO, "dx=%d, dz=%d", dx, dz);

  int camera_chunk_pos_x = last_camera_chunk_pos_x;
  int camera_chunk_pos_z = last_camera_chunk_pos_z;

  for (int i = 0; i < dx; i++) {
    camera_chunk_pos_x = last_camera_chunk_pos_x + i;
    doIterate(camera_chunk_pos_x, camera_chunk_pos_z);
    m_camera_last_chunk_pos.x = camera_chunk_pos_x;
  }

  for (int j = 0; j < dz; j++) {
    camera_chunk_pos_z = last_camera_chunk_pos_z + j;
    doIterate(camera_chunk_pos_x, camera_chunk_pos_z);
    m_camera_last_chunk_pos.z = camera_chunk_pos_z;
  }
}

void ChunkRenderer::doIterate(int camera_chunk_pos_x, int camera_chunk_pos_z) {
  ChunkBorder chunk_border;
  uint8_t max_lod_level = LevelOfDetail::getMaxLodLevel();
  // for (int i = 1; i < max_lod_level; i++) {
  //	int border_dist = LevelOfDetail::Lods[i].draw_distance / 2;
  //	chunk_border.min_x = camera_chunk_pos_x - border_dist;
  //	chunk_border.max_x = camera_chunk_pos_x + border_dist;
  //	chunk_border.min_z = camera_chunk_pos_z - border_dist;
  //	chunk_border.max_z = camera_chunk_pos_z + border_dist;
  //	iterateOverChunkBorderAndUpdateLod(chunk_border);
  // }

  int border_dist = Settings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2;
  chunk_border.min_x = camera_chunk_pos_x - border_dist;
  chunk_border.max_x = camera_chunk_pos_x + border_dist - 1;
  chunk_border.min_z = camera_chunk_pos_z - border_dist;
  chunk_border.max_z = camera_chunk_pos_z + border_dist - 1;

  WindowMovementDirection move_dir =
      m_chunks_by_coord.getWindowLatestMoveDir(chunk_border);
  LOG_F(INFO, "Move Dir x_p=%d, x_n=%d, z_p=%d, z_n=%d", move_dir.x_p,
        move_dir.x_n, move_dir.z_p, move_dir.z_n);
  m_tasks.emplace(
      [this, move_dir] { iterateOverChunkBorderAndDelete(move_dir); });
  //m_tasks.emplace([this] { updateChunkPipeline(); });
  //m_tasks.emplace([this] { deleteOutOfRenderDistanceChunks(); });
  m_tasks.emplace(
      [this, move_dir] { iterateOverChunkBorderAndCreate(move_dir); });
  m_tasks.emplace([this] { updateChunkPipeline(); });
  m_tasks.emplace([this, chunk_border] { m_chunks_by_coord.moveWindow(chunk_border); });
}

void ChunkRenderer::iterateOverChunkBorderAndCreate(
    WindowMovementDirection move_dir) {
  ChunkBorder chunk_border = m_chunks_by_coord.getBorder();
  int min_x = chunk_border.min_x;
  int max_x = chunk_border.max_x;
  int min_z = chunk_border.min_z;
  int max_z = chunk_border.max_z;

  // x-/x+ iterate over z
  for (int cz = min_z; cz <= max_z; cz++) {
    for (int cy = Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1;
         cy >= 0; cy--) {
      if (move_dir.x_n) {
        m_chunks_to_create.push({min_x - 1 , cy, cz});
      }

      if (move_dir.x_p) {
        m_chunks_to_create.push({max_x + 1, cy, cz});
      }
    }
  }

  // z-/z+ iterate over x
  for (int cx = min_x; cx <= max_x; cx++) {
    for (int cy = Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1;
         cy >= 0; cy--) {
      if (move_dir.z_n) {
        m_chunks_to_create.push({cx, cy, min_z - 1});
      }

      if (move_dir.z_p) {
        m_chunks_to_create.push({cx, cy, max_z + 1});
      }
    }
  }
}

void ChunkRenderer::iterateOverChunkBorderAndDelete(
    WindowMovementDirection move_dir) {
  ChunkBorder chunk_border = m_chunks_by_coord.getBorder();
  int min_x = chunk_border.min_x;
  int max_x = chunk_border.max_x;
  int min_z = chunk_border.min_z;
  int max_z = chunk_border.max_z;

  // x-/x+ iterate over z
  for (int cz = min_z; cz <= max_z; cz++) {
    for (int cy = Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1;
         cy >= 0; cy--) {
      if (move_dir.x_n) {
        m_chunks_to_delete.push({max_x, cy, cz});
      }

      if (move_dir.x_p) {
        m_chunks_to_delete.push({min_x, cy, cz});
      }
    }
  }

  // z-/z+ iterate over x
  for (int cx = min_x; cx <= max_x; cx++) {
    for (int cy = Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1;
         cy >= 0; cy--) {
      if (move_dir.z_n) {
        m_chunks_to_delete.push({cx, cy, max_z});
      }

      if (move_dir.z_p) {
        m_chunks_to_delete.push({cx, cy, min_z});
      }
    }
  }
}

void ChunkRenderer::iterateOverChunkBorderAndUpdateLod(
    ChunkBorder chunk_border) {
  int min_x = chunk_border.min_x;
  int max_x = chunk_border.max_x;
  int min_z = chunk_border.min_z;
  int max_z = chunk_border.max_z;

  // x-/x+ iterate over z
  for (int cz = min_z; cz < max_z; cz++) {
    for (int cy = Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1;
         cy >= 0; cy--) {
      if (checkIfChunkLodNeedsUpdate({max_x, cy, cz})) {
        m_chunks_to_delete.push({max_x, cy, cz});
        m_chunks_to_create.push({max_x, cy, cz});
      }

      if (checkIfChunkLodNeedsUpdate({min_x - 1, cy, cz})) {
        m_chunks_to_delete.push({min_x - 1, cy, cz});
        m_chunks_to_create.push({min_x - 1, cy, cz});
      }
    }
  }

  // z-/z+ iterate over x
  for (int cx = min_x; cx < max_x; cx++) {
    for (int cy = Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1;
         cy >= 0; cy--) {
      if (checkIfChunkLodNeedsUpdate({cx, cy, max_z})) {
        m_chunks_to_delete.push({cx, cy, max_z});
        m_chunks_to_create.push({cx, cy, max_z});
      }

      if (checkIfChunkLodNeedsUpdate({cx, cy, min_z - 1})) {
        m_chunks_to_delete.push({cx, cy, min_z - 1});
        m_chunks_to_create.push({cx, cy, min_z - 1});
      }
    }
  }
  updateChunkPipeline();
}

bool ChunkRenderer::isChunkOutOfBorder(glm::ivec3 chunk_pos,
                                       ChunkBorder chunk_border) {
  int cx = chunk_pos.x;
  int cz = chunk_pos.z;

  return cx < chunk_border.min_x || cx > chunk_border.max_x ||
         cz < chunk_border.min_z || cz > chunk_border.max_z;
}

// main thread
void ChunkRenderer::updateBufferIfNeedsUpdate() {
  if (m_buffer_needs_update.load()) {
    LOG_F(INFO, "updateBufferIfNeedsUpdate");
    m_buffer_needs_update.store(false);
    // free should go first, before allocate
    freeChunks();
    allocateChunks();
    m_vertexpool->createChunkInfoBuffer();
    m_vertexpool->createChunkLodBuffer();
  }
}

void ChunkRenderer::runTraverseSceneInDetachedThread() {
  std::thread(&ChunkRenderer::traverseSceneLoop, this).detach();
}

// render thread
bool ChunkRenderer::createChunksInRenderDistance() {
  LOG_F(INFO, "chunks to create: %d", m_chunks_to_create.size());
  bool anything_created = false;
  while (!m_chunks_to_create.empty()) {
    glm::ivec3 chunk_pos = m_chunks_to_create.front();
    anything_created |= createChunkIfNotPresent(chunk_pos);
    m_chunks_to_create.pop();
  }

  return anything_created;
}

// render thread
bool ChunkRenderer::createChunkIfNotPresent(glm::ivec3 chunk_pos) {
  std::weak_ptr<Chunk> chunk = m_chunks_by_coord.get(chunk_pos);
  if (chunk.lock() && chunk.lock()->getPos() == chunk_pos) {
    return false;
  }

  createChunk(chunk_pos);
  return true;
}

// render thread
void ChunkRenderer::createChunk(glm::ivec3 chunk_pos) {
  glm::ivec3 camera_pos = Util::worldPosToChunkPos(m_camera.getCameraPos());
  LevelOfDetail::LevelOfDetail lod =
      LevelOfDetail::chooseLevelOfDetail(camera_pos, chunk_pos);
  HeightMap height_map = generateHeightmap(chunk_pos, lod);
  bool is_chunk_visible = !m_terrain_generator.isChunkBelowOrAboveSurface(
      chunk_pos, height_map, lod);

  m_chunks_by_coord.set(chunk_pos, std::make_shared<Chunk>(chunk_pos, lod));
  m_chunks_to_populate_neighbors.push(chunk_pos);
  m_chunks_by_coord.get(chunk_pos).lock()->setState(ChunkState::CREATED);
}

HeightMap ChunkRenderer::generateHeightmap(glm::ivec3 chunk_pos,
    LevelOfDetail::LevelOfDetail lod) {
#if SETTING_USE_HEIGHTMAP_BLENDING
      return m_terrain_generator.generateBlendedHeightMap(chunk_pos, lod);
#elif SETTING_USE_PRELOADED_HEIGHTMAP
      return m_terrain_generator.generatePreloadedHeightMap(chunk_pos, lod);
#else
      return m_terrain_generator.generateProceduralHeightMap(chunk_pos, lod);
#endif
}

bool ChunkRenderer::populateChunksNeighbors() {
  LOG_F(INFO, "chunks to populate neighbors: %d",
        m_chunks_to_populate_neighbors.size());
  bool anything_populated = false;
  while (!m_chunks_to_populate_neighbors.empty()) {
    glm::ivec3 chunk_pos = m_chunks_to_populate_neighbors.front();
    anything_populated |= populateChunkNeighbor(chunk_pos);
    m_chunks_to_populate_neighbors.pop();
  }
  return anything_populated;
}

bool ChunkRenderer::populateChunkNeighbor(glm::ivec3 chunk_pos) {
  ChunkNeighbors chunk_neighbors;
  int x = chunk_pos.x, y = chunk_pos.y, z = chunk_pos.z;

  for (int x_offset : {-1, 0, 1}) {
    for (int y_offset : {-1, 0, 1}) {
      for (int z_offset : {-1, 0, 1}) {
        glm::ivec3 target_chunk_pos = {x + x_offset, y + y_offset,
                                       z + z_offset};
        chunk_neighbors.push_back(
            {target_chunk_pos, m_chunks_by_coord.get(target_chunk_pos)});
      }
    }
  }
  m_chunks_by_coord.get(chunk_pos).lock()->setNeighbors(chunk_neighbors);
  m_chunks_by_coord.get(chunk_pos).lock()->setState(ChunkState::NEIGHBORS_POPULATED);
  m_chunks_to_generate_terrain.push(chunk_pos);

  return true;
}

bool ChunkRenderer::generateChunksTerrain() {
  LOG_F(INFO, "chunks to generate terrain: %d",
        m_chunks_to_generate_terrain.size());
  bool anything_generated = false;
#if SETTING_USE_PRELOADED_HEIGHTMAP
  std::queue<glm::ivec3> chunks_to_generate_underground_layer;
  while (!m_chunks_to_generate_terrain.empty()) {
    glm::ivec3 chunk_pos = m_chunks_to_generate_terrain.front();
    anything_generated |= generateChunkTerrain(chunk_pos);
    m_chunks_to_generate_terrain.pop();
    chunks_to_generate_underground_layer.push(chunk_pos);
  }
#else
  while (!m_chunks_to_generate_terrain.empty()) {
    glm::ivec3 chunk_pos = m_chunks_to_generate_terrain.front();
    anything_generated |= generateChunkTerrain(chunk_pos);
    m_chunks_to_generate_terrain.pop();
  }
#endif
  return anything_generated;
}

bool ChunkRenderer::generateChunkTerrain(glm::ivec3 chunk_pos) {
  glm::ivec3 camera_pos = Util::worldPosToChunkPos(m_camera.getCameraPos());
  LevelOfDetail::LevelOfDetail lod =
      LevelOfDetail::chooseLevelOfDetail(camera_pos, chunk_pos);

#if SETTING_USE_HEIGHTMAP_BLENDING
  HeightMap height_map =
      m_terrain_generator.generateBlendedHeightMap(chunk_pos, lod);
#elif SETTING_USE_PRELOADED_HEIGHTMAP
  HeightMap height_map =
      m_terrain_generator.generatePreloadedHeightMap(chunk_pos, lod);
#else
  ProceduralHeightMap height_map =
      m_terrain_generator.generateProceduralHeightMap(chunk_pos, lod);
#endif
  bool is_chunk_visible = !m_terrain_generator.isChunkBelowOrAboveSurface(
      chunk_pos, height_map, lod);
  if (!is_chunk_visible) {
    return false;
  }

#if SETTING_USE_PRELOADED_COLORMAP
  std::weak_ptr<Chunk> chunk = m_chunks_by_coord.get(chunk_pos);
  m_terrain_generator.generatePreloadedLayers(*chunk.lock(), height_map);
  m_chunks_to_decorate.push(chunk_pos);
  chunk.lock()->setState(ChunkState::TERRAIN_GENERATED);
#else
  m_terrain_generator.generateChunkTerrain(*m_chunks_by_coord.get(chunk_pos).lock(),
                                           height_map, is_chunk_visible);
  m_chunks_to_decorate.push(chunk_pos);
  m_chunks_by_coord.get(chunk_pos).lock()->setState(ChunkState::TERRAIN_GENERATED);

#endif
  return true;
}

// render thread
bool ChunkRenderer::decorateChunkIfPresent(glm::ivec3 chunk_pos) {
  std::weak_ptr<Chunk> chunk = m_chunks_by_coord.get(chunk_pos);
#if SETTING_TREES_ENABLED
  m_terrain_generator.generateTrees(*chunk);
#endif
  m_chunks_to_mesh.push(chunk_pos);
  chunk.lock()->setState(ChunkState::DECORATED);

  return true;
}

// render thread
bool ChunkRenderer::decorateChunks() {
  LOG_F(INFO, "chunks to decorate: %d", m_chunks_to_decorate.size());
  bool anything_decorated = false;
  while (!m_chunks_to_decorate.empty()) {
    glm::ivec3 chunk_pos = m_chunks_to_decorate.front();
    anything_decorated |= decorateChunkIfPresent(chunk_pos);
    m_chunks_to_decorate.pop();
  }
  return anything_decorated;
}

// render thread
bool ChunkRenderer::meshChunks() {
  LOG_F(INFO, "chunks to mesh: %d", m_chunks_to_mesh.size());
  bool anything_meshed = false;
  while (!m_chunks_to_mesh.empty()) {
    glm::ivec3 chunk_pos = m_chunks_to_mesh.front();
    anything_meshed |= meshChunk(chunk_pos);
    m_chunks_to_mesh.pop();
    m_chunks_to_allocate.push(chunk_pos);
  }
  return anything_meshed;
}

// render thread
bool ChunkRenderer::meshChunk(glm::ivec3 chunk_pos) {
  std::weak_ptr<Chunk> chunk = m_chunks_by_coord.get(chunk_pos);
  if (chunk.lock()->isVisible()) {
    chunk.lock()->addChunkMesh();
  }
  chunk.lock()->setState(ChunkState::MESHED);

  return true;
}

// render thread
bool ChunkRenderer::deleteOutOfRenderDistanceChunks() {
  LOG_F(INFO, "chunks to delete: %d", m_chunks_to_delete.size());
  bool anything_deleted = false;
  while (!m_chunks_to_delete.empty()) {
    glm::ivec3 chunk_pos = m_chunks_to_delete.front();
    anything_deleted |= deleteChunkIfPresent(chunk_pos);
    m_chunks_to_delete.pop();
  }
  return anything_deleted;
}

// render thread
bool ChunkRenderer::deleteChunkIfPresent(glm::ivec3 chunk_pos) {
  auto current_chunk = m_chunks_by_coord.get(chunk_pos).lock();
  if (!current_chunk || current_chunk.get()->getPos() != chunk_pos) return false;

  deleteChunk(chunk_pos);
  return true;
}

// render thread
void ChunkRenderer::deleteChunk(glm::ivec3 chunk_pos) {
  m_chunks_to_free.push(chunk_pos);
}

// render thread
bool ChunkRenderer::checkIfChunkLodNeedsUpdate(glm::ivec3 chunk_pos) {
  glm::ivec3 camera_pos = Util::worldPosToChunkPos(m_camera.getCameraPos());
  LevelOfDetail::LevelOfDetail lod =
      LevelOfDetail::chooseLevelOfDetail(camera_pos, chunk_pos);
  std::weak_ptr<Chunk> chunk = m_chunks_by_coord.get(chunk_pos);
  return !chunk.lock() && chunk.lock()->getLevelOfDetail().level != lod.level;
}

// main thread
void ChunkRenderer::allocateChunks() {
  LOG_F(INFO, "allocateChunks");
  while (!m_chunks_to_allocate.empty()) {
    allocateChunk(m_chunks_to_allocate.front());
    m_chunks_to_allocate.pop();
  }
}

// main thread
void ChunkRenderer::allocateChunk(glm::ivec3 chunk_pos) {
  std::weak_ptr<Chunk> chunk = m_chunks_by_coord.get(chunk_pos);
  VertexPool::ChunkAllocData alloc_data;

  alloc_data._chunk_pos = chunk.lock()->getPos();
  alloc_data._added_faces_amount = chunk.lock()->getAddedFacesAmount();
  alloc_data._lod = chunk.lock()->getLevelOfDetail();
  alloc_data._mesh = std::move(
      chunk.lock()->getMesh());  // chunk mesh is about to be allocated,
                                    // vertex pool takes ownership
  alloc_data._mesh_faces = std::move(chunk.lock()->getFaces());
  alloc_data._chunk_world_pos = chunk.lock()->getWorldPos();
  alloc_data._ready = true;

  chunk.lock()->setState(ChunkState::ALLOCATED);
  m_vertexpool->allocate(std::move(alloc_data));

}

// main thread
void ChunkRenderer::freeChunks() {
  LOG_F(INFO, "freeChunks");
  while (!m_chunks_to_free.empty()) {
    glm::ivec3 chunk_pos = m_chunks_to_free.front();
    freeChunk(chunk_pos);
    m_chunks_to_free.pop();
  }
}

// main thread
void ChunkRenderer::freeChunk(glm::ivec3 chunk_pos) {
  m_vertexpool->free(chunk_pos);
}

void ChunkRenderer::updateChunkPipeline() {
  if (m_chunks_to_create.size() > 0 || m_chunks_to_delete.size() > 0) {
    bool result = deleteOutOfRenderDistanceChunks();
    result |= createChunksInRenderDistance();
    result |= populateChunksNeighbors();
    result |= generateChunksTerrain();
    result |= decorateChunks();
    result |= meshChunks();
    result |= m_buffer_needs_update;
    m_buffer_needs_update.store(result);
  }
}
