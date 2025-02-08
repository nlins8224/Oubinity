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
  LOG_F(INFO, "Generation threads amount=%d",
        m_generation_task_pool.get_thread_count());
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

// generation thread
void ChunkRenderer::traverseSceneLoop() {
  while (true) {
    if (m_generation_tasks.load() != 0 || m_free_tasks.load() != 0) {
      m_buffer_needs_update.store(true);
      continue;
      //return;
    }
    traverseScene();
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
  glm::ivec3 local_pos = Util::chunkWorldPosToPaddedLocalPos(world_block_pos);
  return chunk.lock()->getBlockId(local_pos);
}

bool ChunkRenderer::isBlockPresentByWorldPos(glm::ivec3 world_block_pos) {
  std::weak_ptr<Chunk> chunk = getChunkByWorldPos(world_block_pos);
  if (!chunk.lock()) {
    return block_id::NONE;
  }
  glm::ivec3 local_pos = Util::chunkWorldPosToPaddedLocalPos(world_block_pos);
  if (!chunk.lock()->getState().has_blocks) {
    glm::ivec3 chunk_pos = Util::worldPosToChunkPos(world_block_pos);
    bool result = generateChunkTerrainIfNeeded(chunk_pos);
    if (!result) {
      return false;
    }
  }
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
  if (!chunk.lock()->getState().has_blocks) {
    glm::ivec3 chunk_pos = Util::worldPosToChunkPos(world_block_pos);
    bool result = generateChunkTerrainIfNeeded(chunk_pos);
    LOG_F(INFO, "generateChunkTerrainIfNeeded");
    if (!result) {
      return;
    }
  } else {
    LOG_F(WARNING, "CHUNK HAS BLOCKS");
  }
  chunk.lock()->setBlock(Util::chunkWorldPosToPaddedLocalPos(world_block_pos),
                         type);
  chunk.lock()->setChunkEditedState(true);
  freeChunk(chunk_pos, true);
  meshChunk(chunk_pos);
  allocateChunk(getAllocData(chunk_pos), true);
  m_vertexpool->createChunkInfoBuffer();
  m_vertexpool->createChunkLodBuffer();
}

// main thread
void ChunkRenderer::updateBufferIfNeedsUpdate() {
  glm::ivec3 chunk_pos;
  if (m_buffer_needs_update) {
    LOG_F(INFO, "Updating buffer");
    bool updated = false;

    while (m_chunks_to_free.try_dequeue(chunk_pos)) {
      updated = true;
      freeChunk(chunk_pos, false);
      m_free_tasks.fetch_add(-1);
    }

    VertexPool::ChunkAllocData alloc_data;
    bool gen_items_left;

    while (m_chunks_to_allocate.try_dequeue(alloc_data)) {
      updated = true;
      allocateChunk(alloc_data, false);
      m_generation_tasks.fetch_add(-1);
    }

    if (updated) {
      m_vertexpool->commitUpdate();
    }
    // LOG_F(INFO, "Free items left=%d, Gen items left=%d", free_items_left,
    //       gen_items_left);
    m_buffer_needs_update.store(false);
  }
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
        generateChunk({cx, cy, cz});
      }
    }
  }
  m_buffer_needs_update.store(true);
  updateBufferIfNeedsUpdate();
  m_init_stage = false;
}

void ChunkRenderer::traverseScene() {
  glm::ivec3 dst_camera_chunk_pos =
      Util::worldPosToChunkPos(m_camera.getCameraPos());

  int dx = dst_camera_chunk_pos.x - m_camera_last_chunk_pos.x;
  int dz = dst_camera_chunk_pos.z - m_camera_last_chunk_pos.z;
  if (dx == 0 && dz == 0) {
    return;
  }
  if (dx > 1 || dz > 1 || dx < -1 || dz < -1) {
    LOG_F(WARNING,
          "Delta between last and current camera pos dx: %d, dz: %d, but "
          "should be +/- 1",
          dx, dz);
  }
  LOG_F(INFO, "dx=%d, dz=%d", dx, dz);

  if (dx != 0 && dz != 0) {
    LOG_F(ERROR, "TODO: Fix dx==%d, dz==%d traverse case", dx, dz);
    doIterate(m_camera_last_chunk_pos.x, m_camera_last_chunk_pos.z,
              m_camera_last_chunk_pos.x, dst_camera_chunk_pos.z);
    m_camera_last_chunk_pos.z = dst_camera_chunk_pos.z;
    doIterate(m_camera_last_chunk_pos.x, m_camera_last_chunk_pos.z,
              dst_camera_chunk_pos.x, dst_camera_chunk_pos.z);
    m_camera_last_chunk_pos.x = dst_camera_chunk_pos.x;
  } else {
    doIterate(m_camera_last_chunk_pos.x,
              m_camera_last_chunk_pos.z, dst_camera_chunk_pos.x,
              dst_camera_chunk_pos.z);
    m_camera_last_chunk_pos.x = dst_camera_chunk_pos.x;
    m_camera_last_chunk_pos.z = dst_camera_chunk_pos.z;
  }
}

void ChunkRenderer::doIterate(int src_camera_chunk_pos_x, int src_camera_chunk_pos_z, int dst_camera_chunk_pos_x, int dst_camera_chunk_pos_z) {
  ChunkBorder dst_chunk_border;
  uint8_t max_lod_level = LevelOfDetail::getMaxLodLevel();
   //for (int i = 1; i <= max_lod_level; i++) {
  	//int border_dist = LevelOfDetail::Lods[i].draw_distance / 2;
  	//chunk_border.min_x = src_camera_chunk_pos_x - border_dist + dx;
  	//chunk_border.max_x = src_camera_chunk_pos_x + border_dist + dx - 1;
  	//chunk_border.min_z = src_camera_chunk_pos_z - border_dist + dz;
  	//chunk_border.max_z = src_camera_chunk_pos_z + border_dist + dz - 1;
   //     LOG_F(1,
   //           "LOD update border: min_x=%d, max_x=%d, min_z=%d, max_z=%d, "
   //           "border_dist=%d",
   //           chunk_border.min_x, chunk_border.max_x, chunk_border.min_z,
   //           chunk_border.max_z, border_dist);
  	//iterateOverChunkBorderAndUpdateLod(chunk_border);
   //}

  int border_dist = Settings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 2;
  dst_chunk_border.min_x = dst_camera_chunk_pos_x - border_dist;
  dst_chunk_border.max_x = dst_camera_chunk_pos_x + border_dist - 1;
  dst_chunk_border.min_z = dst_camera_chunk_pos_z - border_dist;
  dst_chunk_border.max_z = dst_camera_chunk_pos_z + border_dist - 1;

  ChunkBorder src_chunk_border;
  src_chunk_border.min_x = src_camera_chunk_pos_x - border_dist;
  src_chunk_border.max_x = src_camera_chunk_pos_x + border_dist - 1;
  src_chunk_border.min_z = src_camera_chunk_pos_z - border_dist;
  src_chunk_border.max_z = src_camera_chunk_pos_z + border_dist - 1;

  WindowMovementDirection move_dir =
      m_chunks_by_coord.getWindowMoveDir(src_chunk_border, dst_chunk_border);
  LOG_F(INFO, "Move Dir x_p=%d, x_n=%d, z_p=%d, z_n=%d", move_dir.x_p,
        move_dir.x_n, move_dir.z_p, move_dir.z_n);
  //iterateOverChunkBorderAndDelete(move_dir, dst_chunk_border);
  iterateOverChunkBorderAndCreate(move_dir, dst_chunk_border);

  LOG_F(INFO, "camera_last_chunk_pos updated (%d, %d)",
        m_camera_last_chunk_pos.x, m_camera_last_chunk_pos.z);
}

void ChunkRenderer::iterateOverChunkBorderAndCreate(
    WindowMovementDirection move_dir, ChunkBorder dst_chunk_border) {
  int min_x = dst_chunk_border.min_x;
  int max_x = dst_chunk_border.max_x;
  int min_z = dst_chunk_border.min_z;
  int max_z = dst_chunk_border.max_z;
  glm::ivec3 pos_gen;
  glm::ivec3 pos_free;
  // x-/x+ iterate over z
  for (int cz = min_z; cz <= max_z; cz++) {
    for (int cy = Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1;
         cy >= 0; cy--) {
      if (move_dir.x_n) {
        pos_gen = {min_x, cy, cz};
        pos_free = {max_x + 1, cy, cz};
       //m_generation_task_pool.push_task([this, pos] {
       //  generateChunk({pos.x, pos.y, pos.z}); 
       //    });
        freeChunkIfPresent(pos_free);
        generateChunk(pos_gen);
       }

      if (move_dir.x_p) {
         pos_gen = {max_x, cy, cz};
         pos_free = {min_x - 1, cy, cz};
        // m_generation_task_pool.push_task([this, pos] {
        //   generateChunk({pos.x, pos.y, pos.z});
        //     });
         freeChunkIfPresent(pos_free);
         generateChunk(pos_gen);
      }
    }
  }

  // z-/z+ iterate over x
  for (int cx = min_x; cx <= max_x; cx++) {
    for (int cy = Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1;
         cy >= 0; cy--) {

      if (move_dir.z_n) {
        // m_generation_task_pool.push_task([this, pos] {
        //   generateChunk({pos.x, pos.y, pos.z});
        //     });
        pos_gen = {cx, cy, min_z};
        pos_free = {cx, cy, max_z + 1};
        freeChunkIfPresent(pos_free);
        generateChunk(pos_gen);
      }

      if (move_dir.z_p) {
        // m_generation_task_pool.push_task([this, pos] {
        //   generateChunk({pos.x, pos.y, pos.z});
        //     });
        pos_gen = {cx, cy, max_z};
        pos_free = {cx, cy, min_z - 1};
        freeChunkIfPresent(pos_free);
        generateChunk(pos_gen);
      }
    }
  }
}

void ChunkRenderer::iterateOverChunkBorderAndDelete(
    WindowMovementDirection move_dir, ChunkBorder dst_chunk_border) {
  int min_x = dst_chunk_border.min_x;
  int max_x = dst_chunk_border.max_x;
  int min_z = dst_chunk_border.min_z;
  int max_z = dst_chunk_border.max_z;
  glm::ivec3 pos;
  // x-/x+ iterate over z
  for (int cz = min_z; cz <= max_z; cz++) {
    for (int cy = Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1;
         cy >= 0; cy--) {
      if (move_dir.x_n) {
        pos = {max_x + 1, cy, cz};
        //m_generation_task_pool.push_task(
        //    [this, pos] { freeChunkIfPresent(pos); });
        freeChunkIfPresent(pos); 
      }

      if (move_dir.x_p) {
        pos = {min_x - 1, cy, cz};
        // m_generation_task_pool.push_task(
        //     [this, pos] { freeChunkIfPresent(pos); });
        freeChunkIfPresent(pos); 
      }
    }
  }

  // z-/z+ iterate over x
  for (int cx = min_x; cx <= max_x; cx++) {
    for (int cy = Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1;
         cy >= 0; cy--) {
      if (move_dir.z_n) {
        pos = {cx, cy, max_z + 1};
        // m_generation_task_pool.push_task(
        //     [this, pos] { freeChunkIfPresent(pos); });
        freeChunkIfPresent(pos); 
      }

      if (move_dir.z_p) {
        pos = {cx, cy, min_z - 1};
        // m_generation_task_pool.push_task(
        //     [this, pos] { freeChunkIfPresent(pos); });
        freeChunkIfPresent(pos); 
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

  glm::ivec3 pos;

  // x-/x+ iterate over z
  for (int cz = min_z; cz <= max_z; cz++) {
    for (int cy = Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1;
         cy >= 0; cy--) {

      pos = {max_x, cy, cz};
      if (markIfChunkLodNeedsUpdate(pos)) {
        m_generation_task_pool.push_task([this, pos] {
          freeChunkIfPresent(pos);
          generateChunk(pos);
        });
      }

      pos = {min_x, cy, cz};
      if (markIfChunkLodNeedsUpdate(pos)) {
        m_generation_task_pool.push_task([this, pos] {
          freeChunkIfPresent(pos);
          generateChunk(pos);
        });
      }
    }
  }

  std::unordered_set<glm::ivec2> already_traversed = {
      {min_x, min_z}, {min_x, max_z}, {max_x, min_z}, {max_x, max_z}};

  // z-/z+ iterate over x
  for (int cx = min_x; cx <= max_x; cx++) {
    for (int cy = Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - 1;
         cy >= 0; cy--) {
      pos = {cx, cy, max_z};
      if (!already_traversed.contains({pos.x, pos.z}) && markIfChunkLodNeedsUpdate(pos)) {
        m_generation_task_pool.push_task([this, pos] {
          freeChunkIfPresent(pos);
          generateChunk(pos);
        });
      }

      pos = {cx, cy, min_z};
      if (!already_traversed.contains({pos.x, pos.z}) && markIfChunkLodNeedsUpdate(pos)) {
        m_generation_task_pool.push_task([this, pos] {
          freeChunkIfPresent(pos);
          generateChunk(pos);
        });
      }
    }
  }
}

bool ChunkRenderer::isChunkOutOfBorder(glm::ivec3 chunk_pos,
                                       ChunkBorder chunk_border) {
  int cx = chunk_pos.x;
  int cz = chunk_pos.z;

  return cx < chunk_border.min_x || cx > chunk_border.max_x ||
         cz < chunk_border.min_z || cz > chunk_border.max_z;
}

void ChunkRenderer::generateChunk(glm::ivec3 chunk_pos) {
  bool chunk_created = createChunkIfNotPresent(chunk_pos);
  if (!chunk_created && !m_chunks_by_coord.get(chunk_pos).lock()->getState().needs_lod_update) {
    return;
  }   
  auto chunk = m_chunks_by_coord.get(chunk_pos).lock();
  CHECK_F(!chunk->isGenerationRunning(),
          "Generation for (%d, %d, %d) is already running on another thread",
          chunk_pos.x, chunk_pos.y, chunk_pos.z);
  chunk->setIsGenerationRunning(true);
  ChunkState chunk_state = m_chunks_by_coord.get(chunk_pos).lock()->getState();
  if (chunk_state.needs_lod_update) {
    chunk->setChunkHasBlocksState(false);
  }

  bool terrain_generated = generateChunkTerrainIfNeeded(chunk_pos);
  if (!terrain_generated) {
    chunk->setIsGenerationRunning(false);
    return;
  }
  bool mesh_chunk = meshChunk(chunk_pos);
  if (!mesh_chunk) {
    chunk->setIsGenerationRunning(false);
    return;
  }
  m_generation_tasks.fetch_add(1);
  m_chunks_to_allocate.enqueue(getAllocData(chunk_pos));
  m_chunks_by_coord.get(chunk_pos).lock()->setChunkNeedsLodUpdate(false);
  chunk->setIsGenerationRunning(false);
}

// generation thread
VertexPool::ChunkAllocData ChunkRenderer::getAllocData(glm::ivec3 chunk_pos) {
  std::weak_ptr<Chunk> chunk = m_chunks_by_coord.get(chunk_pos);
  VertexPool::ChunkAllocData alloc_data;
  alloc_data._chunk_pos = chunk.lock()->getPos();
  alloc_data._added_faces_amount = chunk.lock()->getAddedFacesAmount();
  alloc_data._lod = chunk.lock()->getLevelOfDetail();
  alloc_data._mesh = chunk.lock()->getMesh();                            
  alloc_data._mesh_faces = chunk.lock()->getFaces();
  alloc_data._chunk_world_pos = chunk.lock()->getWorldPos();
  alloc_data._ready = true;
  return alloc_data;
}

void ChunkRenderer::runTraverseSceneInDetachedThread() {
  std::thread(&ChunkRenderer::traverseSceneLoop, this).detach();
}


// generation thread
bool ChunkRenderer::createChunkIfNotPresent(glm::ivec3 chunk_pos) {
  std::weak_ptr<Chunk> chunk = m_chunks_by_coord.get(chunk_pos);
  if (chunk.lock() && chunk.lock()->getPos() == chunk_pos) {
    return false;
  }

  createChunk(chunk_pos);
  return true;
}

// generation thread
void ChunkRenderer::createChunk(glm::ivec3 chunk_pos) {
  glm::ivec3 camera_pos = Util::worldPosToChunkPos(m_camera.getCameraPos());
  LevelOfDetail::LevelOfDetail lod =
      LevelOfDetail::chooseLevelOfDetail(camera_pos, chunk_pos);
  HeightMap height_map = generateHeightmap(chunk_pos, lod);
  m_chunks_by_coord.set(chunk_pos, std::make_shared<Chunk>(chunk_pos, lod));
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
  return true;
}

bool ChunkRenderer::generateChunkTerrainIfNeeded(glm::ivec3 chunk_pos) {
  glm::ivec3 camera_pos = Util::worldPosToChunkPos(m_camera.getCameraPos());
  LevelOfDetail::LevelOfDetail lod =
      LevelOfDetail::chooseLevelOfDetail(camera_pos, chunk_pos);
  std::weak_ptr<Chunk> chunk = m_chunks_by_coord.get(chunk_pos);
  if (chunk.lock()->getState().has_blocks) {
    return false;
  }

  chunk.lock()->setLevelOfDetail(lod);

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
  m_terrain_generator.generatePreloadedLayers(*chunk.lock(), height_map);
#else
  m_terrain_generator.generateChunkTerrain(*chunk.lock(),
                                           height_map, is_chunk_visible);
#endif
  chunk.lock()->setChunkHasBlocksState(true);
  return true;
}

// generation thread
bool ChunkRenderer::decorateChunkIfPresent(glm::ivec3 chunk_pos) {
  std::weak_ptr<Chunk> chunk = m_chunks_by_coord.get(chunk_pos);
#if SETTING_TREES_ENABLED
  m_terrain_generator.generateTrees(*chunk);
#endif

  return true;
}

// generation thread
bool ChunkRenderer::meshChunk(glm::ivec3 chunk_pos) {
  std::weak_ptr<Chunk> chunk = m_chunks_by_coord.get(chunk_pos);
  if (chunk.lock()->isVisible()) {
    chunk.lock()->addChunkMesh();
  }
  return true;
}

// generation thread
bool ChunkRenderer::freeChunkIfPresent(glm::ivec3 chunk_pos) {
  auto current_chunk = m_chunks_by_coord.get(chunk_pos).lock();
  if (!current_chunk || current_chunk.get()->getPos() != chunk_pos) return false;

  m_chunks_to_free.enqueue(chunk_pos);
  m_free_tasks.fetch_add(1);
  return true;
}

// generation thread
bool ChunkRenderer::markIfChunkLodNeedsUpdate(glm::ivec3 chunk_pos) {
  glm::ivec3 camera_pos = Util::worldPosToChunkPos(m_camera.getCameraPos());
  LevelOfDetail::LevelOfDetail lod =
      LevelOfDetail::chooseLevelOfDetail(camera_pos, chunk_pos);
  std::weak_ptr<Chunk> chunk = m_chunks_by_coord.get(chunk_pos);
  if (!chunk.lock()) {
    return false;
  }
  bool needs_update = chunk.lock()->getLevelOfDetail().level != lod.level;
  chunk.lock()->setChunkNeedsLodUpdate(needs_update);
  return needs_update;
}

// main thread
void ChunkRenderer::allocateChunk(VertexPool::ChunkAllocData alloc_data, bool fast_path) {
  m_vertexpool->push_allocate(std::move(alloc_data), fast_path);
}

// main thread
void ChunkRenderer::freeChunk(glm::ivec3 chunk_pos, bool fast_path) {
  m_vertexpool->push_free(chunk_pos, fast_path);
}