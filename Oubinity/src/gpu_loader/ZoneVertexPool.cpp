#include "ZoneVertexPool.h"

namespace VertexPool {
ZoneVertexPool::ZoneVertexPool()
    : m_mesh_persistent_buffer{nullptr},
      m_face_stream_buffer{nullptr},
      m_persistent_buffer_vertices_amount{0},
      m_mesh_faces_amount{0},
      m_chunk_buckets{zones.size(), std::vector<MeshBucket>(0)},
      m_buffer_needs_update_count{0} {
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  formatVBO();
  glGenBuffers(1, &m_daicbo);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);

  calculateBucketAmountInZones();
  m_persistent_buffer_vertices_amount = calculateTotalVertexAmount();
#if SETTING_USE_VERTEX_MESH
  createMeshBuffer();
#endif
  createDAICBuffer();
  initZones(m_mesh_persistent_buffer);
  initBuckets();

  createFaceStreamBuffer();

  LOG_F(INFO, "Total vertices amount: %zu",
        m_persistent_buffer_vertices_amount);
  LOG_F(INFO, "Total faces amount: %zu", m_mesh_faces_amount);

  m_stats.added_faces = 0;
}

void ZoneVertexPool::allocate(ChunkAllocData&& alloc_data) {
  unsigned int added_faces = alloc_data._added_faces_amount;
  glm::ivec3 chunk_pos = alloc_data._chunk_pos;
  if (added_faces == 0) {
    LOG_F(INFO, "Empty chunk at pos (%d, %d, %d), no faces added", chunk_pos.x,
          chunk_pos.y, chunk_pos.z);
    return;
  }

  m_stats.added_faces += added_faces;
  unsigned int added_vertices = Block::VERTICES_PER_FACE * added_faces;
  size_t lod_level = alloc_data._lod.level;
  Zone zone = chooseZone(lod_level);
  MeshBucket* first_free_bucket = getFirstFreeBucket(zone.level);
  m_stats.chunks_in_buckets[zone.level]++;

  if (first_free_bucket == nullptr) {
    LOG_F(ERROR,
          "ALL BUCKETS AT ZONE: %d ARE FULL, NO SPACE LEFT. Tried to add %d "
          "vertices, chunk pos (%d, %d, %d)",
          zone.level, added_vertices, chunk_pos.x, chunk_pos.y, chunk_pos.z);
    return;
  }

  if ((first_free_bucket->_start_offset % Block::VERTICES_PER_FACE) != 0) {
    LOG_F(ERROR, "Offset % %d != 0, start offset: %d", Block::VERTICES_PER_FACE,
          first_free_bucket->_start_offset);
    return;
  }

  LevelOfDetail::LevelOfDetail lod = alloc_data._lod;
  size_t id = first_free_bucket->_id;
  LOG_F(1,
        "Allocating bucket at level: %d, with id: %d, vertex offset: %d, "
        "vertices: %d, faces: %d, at chunk pos: (%d, %d, %d)",
        zone.level, id, first_free_bucket->_start_offset, added_vertices,
        added_faces, chunk_pos.x, chunk_pos.y, chunk_pos.z);
  int vertex_offset = first_free_bucket->_start_offset;
  int face_offset = vertex_offset / Block::VERTICES_PER_FACE;
  if (face_offset == 0) face_offset = 1;
  LOG_F(1,
        "Vertex offset: %d, face offset: %d, vertex offset / face offset: %d",
        vertex_offset, face_offset, vertex_offset / face_offset);

  if (m_chunk_metadata.active_daics.size() > MAX_DAIC_AMOUNT) {
    LOG_F(ERROR,
          "m_chunk_metadata.active_daics.size()=%d, but MAX_DAIC_AMOUNT=%d",
          m_chunk_metadata.active_daics.size(), MAX_DAIC_AMOUNT);
    return;
  }

  m_chunk_metadata.active_daics.emplace_back(
      added_vertices,  // vertices in face * added_faces
      1u, 
      first_free_bucket->_start_offset, // offset in m_mesh_persistent_buffer
      0u);
  first_free_bucket->_is_free = false;

  size_t daic_id = m_chunk_metadata.active_daics.size() - 1;
  LOG_F(1, "daic_id: %d", daic_id);

  m_chunk_metadata.active_chunk_info.chunk_pos[daic_id] = {
      alloc_data._chunk_world_pos, id};
  m_chunk_metadata.active_chunks_lod.chunks_lod[daic_id] =
      static_cast<GLuint>(lod.block_size);
  m_chunk_pos_to_bucket_id[chunk_pos] = {zone.level, id};
  m_bucket_id_to_daic_id[{zone.level, id}] = daic_id;

#if SETTING_USE_VERTEX_MESH
  updateMeshBuffer(alloc_data._mesh, first_free_bucket->_start_offset);
#endif
  updateFaceStreamBuffer(
      alloc_data._mesh_faces,
      first_free_bucket->_start_offset / Block::VERTICES_PER_FACE);
}

void ZoneVertexPool::free(glm::ivec3 chunk_pos) {
  if (m_chunk_pos_to_bucket_id.find(chunk_pos) ==
      m_chunk_pos_to_bucket_id.end()) {
    LOG_F(3, "Chunk at (%d, %d, %d) not found", chunk_pos.x, chunk_pos.y,
          chunk_pos.z);
    return;
  }
  fastErase(chunk_pos);
}

void ZoneVertexPool::fastErase(glm::ivec3 chunk_pos) {
  std::pair<size_t, size_t> bucket_id = m_chunk_pos_to_bucket_id[chunk_pos];
  size_t daic_id = m_bucket_id_to_daic_id[bucket_id];
  size_t last_daic_id = m_chunk_metadata.active_daics.size() - 1;
  std::pair<size_t, size_t> bucket_id_of_last_daic =
      getBucketIdFromDAIC(m_chunk_metadata.active_daics[last_daic_id]);

  LOG_F(5,
        "daic_id: %zu, zone_id: %zu, bucket_id: %zu, last_daic_id: %zu, "
        "last_zone_id: %zu, last_bucket_id: %zu, chunk_pos: (%d, %d, %d)",
        daic_id, bucket_id.first, bucket_id.second, last_daic_id,
        bucket_id_of_last_daic.first, bucket_id_of_last_daic.second,
        chunk_pos.x, chunk_pos.y, chunk_pos.z);

  if (m_chunk_metadata.active_daics.empty() || last_daic_id < daic_id) {
    LOG_F(ERROR,
          "An attempt to delete element %zu was made, but DAIC size is %zu",
          daic_id, last_daic_id);
    return;
  }

  m_chunk_metadata.active_chunk_info.chunk_pos[daic_id] =
      m_chunk_metadata.active_chunk_info.chunk_pos[last_daic_id];
  m_chunk_metadata.active_chunks_lod.chunks_lod[daic_id] =
      m_chunk_metadata.active_chunks_lod.chunks_lod[last_daic_id];
  std::swap(m_chunk_metadata.active_daics[daic_id],
            m_chunk_metadata.active_daics[last_daic_id]);

  m_bucket_id_to_daic_id[bucket_id_of_last_daic] = daic_id;
  m_chunk_buckets[bucket_id.first][bucket_id.second]._is_free = true;
  m_chunk_pos_to_bucket_id.erase(chunk_pos);
  m_chunk_metadata.active_daics.pop_back();
}

// TODO: list of free buckets
MeshBucket* ZoneVertexPool::getFirstFreeBucket(int zone_id) {
  const size_t buckets_amount = zones[zone_id]->buckets_amount;
  for (MeshBucket& bucket : m_chunk_buckets[zone_id]) {
    if (bucket._is_free) return &bucket;
  }
  return nullptr;
}

Zone ZoneVertexPool::chooseZone(unsigned int lod_level) {
  return *zones[lod_level];
}

std::pair<size_t, size_t> ZoneVertexPool::getBucketIdFromDAIC(DAIC daic) {
  Zone zone = calculateZoneFromDaicStartOffset(daic);
  size_t id = (daic.first - zone.start_offset) / zone.max_vertices_per_bucket;
  return {zone.level, id};
}

Zone ZoneVertexPool::calculateZoneFromDaicStartOffset(DAIC daic) {
  for (auto& zone : zones) {
    if (zone->start_offset <= daic.first &&
        daic.first + daic.count < zone->end_offset) {
      return *zone;
    }
  }
}

void ZoneVertexPool::waitBuffer(GLsync& sync) {
  if (sync) {
    while (1) {
      GLenum waitReturn = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
      if (waitReturn == GL_ALREADY_SIGNALED ||
          waitReturn == GL_CONDITION_SATISFIED) {
        return;
      }
    }
  }
}

void ZoneVertexPool::lockBuffer(GLsync& sync) {
  if (sync) {
    glDeleteSync(sync);
  }

  sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

ZoneVertexPool::~ZoneVertexPool() { delete m_mesh_persistent_buffer; }

void ZoneVertexPool::push_allocate(ChunkAllocData&& alloc_data, bool fast_path) {
  if (fast_path) {
    allocate(std::move(alloc_data));
    updateMeshBufferDAIC();
  } else {
    m_pending_allocations.push(alloc_data);
  }

  if (m_pending_allocations.size() >= BUFFER_NEEDS_UPDATE) {
    commitUpdate();
  }
}

void ZoneVertexPool::push_free(glm::ivec3 chunk_pos, bool fast_path) {
  if (fast_path) {
    free(chunk_pos);
  } else {
    m_pending_frees.push(chunk_pos);
  }

  if (m_pending_frees.size() >= BUFFER_NEEDS_UPDATE) {
    commitUpdate();
  }
}

void ZoneVertexPool::commitUpdate() {
    while (!m_pending_frees.empty()) {
      free(m_pending_frees.front());
      m_pending_frees.pop();
    }
    while (!m_pending_allocations.empty()) {
      allocate(std::move(m_pending_allocations.front()));
      m_pending_allocations.pop();
    }
    createChunkInfoBuffer();
    createChunkLodBuffer();
    updateMeshBufferDAIC();
}

void ZoneVertexPool::initBuckets() {
  for (VertexPool::Zone* zone : zones) {
    LOG_F(INFO, "zone buckets amount: %d", zone->buckets_amount);
    for (size_t bucket_idx = 0; bucket_idx < zone->buckets_amount;
         bucket_idx++) {
      MeshBucket bucket{};
      bucket._id = bucket_idx;
      bucket._is_free = true;
      bucket._start_offset =
          zone->start_offset + (bucket_idx * zone->max_vertices_per_bucket);
      m_chunk_buckets[zone->level].push_back(bucket);
    }
  }
}

void ZoneVertexPool::initZones(Vertex* buffer) {
  // TODO: loop
  size_t vertices_in_zero_zone =
      Zero.max_vertices_per_bucket * Zero.buckets_amount;
  Zero.start_offset = 0;
  Zero.end_offset = vertices_in_zero_zone;

  size_t vertices_in_first_zone =
      One.max_vertices_per_bucket * One.buckets_amount;
  One.start_offset = Zero.end_offset;
  One.end_offset = One.start_offset + vertices_in_first_zone;

  size_t vertices_in_second_zone =
      Two.max_vertices_per_bucket * Two.buckets_amount;
  Two.start_offset = One.end_offset;
  Two.end_offset = Two.start_offset + vertices_in_second_zone;

  size_t vertices_in_third_zone =
      Three.max_vertices_per_bucket * Three.buckets_amount;
  Three.start_offset = Two.end_offset;
  Three.end_offset = Three.start_offset + vertices_in_third_zone;

  size_t vertices_in_fourth_zone =
      Four.max_vertices_per_bucket * Four.buckets_amount;
  Four.start_offset = Three.end_offset;
  Four.end_offset = Four.start_offset + vertices_in_fourth_zone;

  Five.start_offset = Four.end_offset;
  Five.end_offset = m_persistent_buffer_vertices_amount;

  LOG_F(INFO, "Zero start offset: %zu, end offset: %zu", Zero.start_offset,
        Zero.end_offset);
  LOG_F(INFO, "One start offset: %zu, end offset: %zu", One.start_offset,
        One.end_offset);
  LOG_F(INFO, "Two start offset: %zu, end offset: %zu", Two.start_offset,
        Two.end_offset);
  LOG_F(INFO, "Three start offset: %zu, end offset: %zu", Three.start_offset,
        Three.end_offset);
  LOG_F(INFO, "Four start offset: %zu, end offset: %zu", Four.start_offset,
        Four.end_offset);
  LOG_F(INFO, "Five start offset: %zu, end offset: %zu", Five.start_offset,
        Five.end_offset);
}

size_t ZoneVertexPool::calculateBucketAmountInZones() {
  size_t buckets_added = 0;
  using Settings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS;
  using LevelOfDetail::Lods;

  auto zones_it = zones.begin();
  auto lods_it = Lods.begin();
  for (zones_it, lods_it; zones_it != zones.end() && lods_it + 1 != Lods.end();
       zones_it++, lods_it++) {
    (*zones_it)->buckets_amount = std::pow((lods_it + 1)->draw_distance, 2) * 4 * Settings::SETTING_BLOCK_MARGIN + ZONE_INITIAL_BUCKET_AMOUNT_MARGIN;
    buckets_added += (*zones_it)->buckets_amount;
    if (lods_it != Lods.end() && lods_it + 1 != Lods.end() &&
        (lods_it + 1)->draw_distance > MAX_RENDERED_CHUNKS_IN_XZ_AXIS + 1) {
      return buckets_added;
    }
  }
  int32_t remaining_buckets = TOTAL_BUCKETS_AMOUNT - buckets_added;
  Five.buckets_amount = std::max(remaining_buckets, 0);
  buckets_added += Five.buckets_amount;
  return buckets_added;
}

size_t ZoneVertexPool::calculateTotalVertexAmount() {
  size_t vertex_amount = 0;

  for (Zone* zone : zones) {
    vertex_amount += zone->max_vertices_per_bucket * zone->buckets_amount;
    LOG_F(INFO,
          "zone %d vertexamount: %d, max_vertices_per_bucket: %d, "
          "buckets_amount: %d",
          zone->level, zone->max_vertices_per_bucket * zone->buckets_amount,
          zone->max_vertices_per_bucket, zone->buckets_amount);
  }
  return vertex_amount;
}

void ZoneVertexPool::formatVBO() {
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glEnableVertexAttribArray(0);
  glVertexAttribIFormat(0, 1, GL_UNSIGNED_BYTE, 0);
  glVertexBindingDivisor(0, 0);
  glVertexAttribBinding(0, 0);
  glBindVertexBuffer(0, m_vbo, offsetof(Vertex, packed_vertex), sizeof(Vertex));
}

void ZoneVertexPool::draw() {
  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_face_stream_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_chunks_lod_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_chunk_info_ssbo);

  glMultiDrawArraysIndirect(
      GL_TRIANGLES,
      (GLvoid*)0, // start with first draw command
      m_chunk_metadata.active_daics.size(),  // DAIC amount
      0                                      // tightly packed
  );
}

void ZoneVertexPool::createMeshBuffer() {
  GLbitfield flags =
      GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  size_t buffer_size = m_persistent_buffer_vertices_amount * sizeof(Vertex);
  LOG_F(INFO,
        "createMeshBuffer buffer_size: %zu, daic size: %zu, OpenGL Error: %d",
        buffer_size, m_chunk_metadata.active_daics.size(), glGetError());
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferStorage(GL_ARRAY_BUFFER, buffer_size, 0, flags);
  m_mesh_persistent_buffer =
      (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, flags);
}

void ZoneVertexPool::createDAICBuffer() {
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
  glBufferData(GL_DRAW_INDIRECT_BUFFER,
               m_chunk_metadata.active_daics.size() * sizeof(DAIC), NULL,
               GL_STREAM_DRAW);
}

void ZoneVertexPool::updateMeshBuffer(std::vector<Vertex>& mesh,
                                      int buffer_offset) {
  waitBuffer(m_sync);
  std::move(mesh.begin(), mesh.end(), m_mesh_persistent_buffer + buffer_offset);
  lockBuffer(m_sync);
}

void ZoneVertexPool::updateMeshBufferDAIC() {
  if (m_chunk_metadata.active_daics.size() == 0) {
    return;  
  }
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
  // Orphan a buffer
  glBufferData(GL_DRAW_INDIRECT_BUFFER,
               m_chunk_metadata.active_daics.size() * sizeof(DAIC), NULL,
               GL_STREAM_DRAW);
  glBufferData(GL_DRAW_INDIRECT_BUFFER,
               m_chunk_metadata.active_daics.size() * sizeof(DAIC),
               m_chunk_metadata.active_daics.data(), GL_STREAM_DRAW);
}

void ZoneVertexPool::createFaceStreamBuffer() {
  GLbitfield flags =
      GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  m_mesh_faces_amount =
      m_persistent_buffer_vertices_amount / Block::VERTICES_PER_FACE;
  GLint buffer_size = m_mesh_faces_amount * sizeof(Face);

  GLint max_ssbo_size;
  glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &max_ssbo_size);

  if (buffer_size > max_ssbo_size) {
    LOG_F(ERROR, "buffer_size: %d, exceeds max_ssbo_size: %d", buffer_size,
          max_ssbo_size);
  }
  LOG_F(INFO, "faces amount: %zu, face stream buffer size: %zu",
        m_mesh_faces_amount, buffer_size);
  m_face_stream_ssbo = 2;

  glGenBuffers(1, &m_face_stream_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_face_stream_ssbo);
  glBufferStorage(GL_SHADER_STORAGE_BUFFER, buffer_size, 0, flags);
  m_face_stream_buffer =
      (Face*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, buffer_size, flags);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_face_stream_ssbo);
}

void ZoneVertexPool::updateFaceStreamBuffer(std::vector<Face>& mesh,
                                            int face_offset) {
  /*
    |waitBuffer| blocks forever on some hardware configurations, not yet sure why is that.
  */                                            
  //waitBuffer(m_face_buffer_sync);
  std::move(mesh.begin(), mesh.end(), m_face_stream_buffer + face_offset);
  //lockBuffer(m_face_buffer_sync);
  LOG_F(1, "face offset: %d, faces amount: %d", face_offset, mesh.size());
}

void ZoneVertexPool::createChunkInfoBuffer() {
  m_chunk_info_ssbo = 0;
  glGenBuffers(1, &m_chunk_info_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_chunk_info_ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               sizeof(m_chunk_metadata.active_chunk_info),
               &m_chunk_metadata.active_chunk_info, GL_STATIC_COPY);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_chunk_info_ssbo);
}

void ZoneVertexPool::createChunkLodBuffer() {
  m_chunks_lod_ssbo = 1;
  glGenBuffers(1, &m_chunks_lod_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_chunks_lod_ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               sizeof(m_chunk_metadata.active_chunks_lod),
               &m_chunk_metadata.active_chunks_lod, GL_STATIC_COPY);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_chunks_lod_ssbo);
}

}  // namespace VertexPool
