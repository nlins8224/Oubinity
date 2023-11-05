#include "VertexPool.h"

VertexPool::VertexPool() :
    m_mesh_persistent_buffer{ nullptr }
{
    initBuckets();
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    formatVBO();
    glGenBuffers(1, &m_daicbo);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
    createMeshBuffer();

    m_stats.max_vertices_occurred = 0;
    m_stats.min_vertices_occurred = PoolConst::MAX_ADDED_VERTICES;
}

void VertexPool::allocate(Chunk& chunk)
{
    unsigned int added_faces = chunk.getAddedFacesAmount();
    if (added_faces == 0)
    {
        glm::ivec3 chunk_pos = chunk.getPos();
        LOG_F(INFO, "Empty chunk chunk at pos (%d, %d, %d), no faces added", chunk_pos.x, chunk_pos.y, chunk_pos.z);
        return;
    }

    MeshBucket* first_free_bucket = getFirstFreeBucket();
    if (first_free_bucket == nullptr)
    {
        LOG_F(WARNING, "ALL BUCKETS ARE FULL, NO SPACE LEFT");
        return;
    }
    DAIC daic
    {
        6 * added_faces, // vertices in face * added_faces
        1,
        first_free_bucket->_start_offset, // offset in m_mesh_persistent_buffer
        0
    };

    LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
    glm::ivec3 chunk_pos = chunk.getPos();
    m_stats.max_vertices_occurred = std::max(m_stats.max_vertices_occurred, (size_t)added_faces * 6);
    m_stats.min_vertices_occurred = std::min(m_stats.min_vertices_occurred, (size_t)added_faces * 6);
    LOG_F(INFO, "max verts: %d, min verts: %d", m_stats.max_vertices_occurred, m_stats.min_vertices_occurred);
    std::vector<Vertex> mesh{ chunk.getMesh().getMeshDataCopy() };
    size_t id = first_free_bucket->_id;
    //LOG_F(INFO, "First free bucket id: %d, start offset: %d, chunk pos: (%d, %d, %d)", id, first_free_bucket->_start_offset, chunk_pos.x, chunk_pos.y, chunk_pos.z);

    m_chunk_metadata.active_daics.push_back(daic);
    first_free_bucket->_is_free = false;
    
    size_t daic_id = m_chunk_metadata.active_daics.size() - 1;
    m_chunk_metadata.active_chunk_info.chunk_pos[daic_id] = { chunk.getWorldPos(), id };
    m_chunk_metadata.active_chunks_lod.chunks_lod[daic_id] = static_cast<GLuint>(lod.block_size);
    m_chunk_pos_to_bucket_id[chunk_pos] = id;
    m_bucket_id_to_daic_id[first_free_bucket->_id] = daic_id;

    updateMeshBuffer(mesh, first_free_bucket->_start_offset);
}

void VertexPool::free(glm::ivec3 chunk_pos)
{
    if (m_chunk_pos_to_bucket_id.find(chunk_pos) == m_chunk_pos_to_bucket_id.end()) {
        LOG_F(WARNING, "Chunk at (%d, %d, %d) not found", chunk_pos.x, chunk_pos.y, chunk_pos.z);
        return;
    }
    fastErase(chunk_pos);
}

void VertexPool::fastErase(glm::ivec3 chunk_pos) {
    size_t bucket_id = m_chunk_pos_to_bucket_id[chunk_pos];
    size_t daic_id = m_bucket_id_to_daic_id[bucket_id];
    size_t last_daic_id = m_chunk_metadata.active_daics.size() - 1;
    size_t bucket_id_of_last_daic = getBucketIdFromStartOffset(m_chunk_metadata.active_daics[last_daic_id].first);

    LOG_F(INFO, "daic_id: %zu, bucket_id: %zu, last_daic_id: %zu, last_bucket_id: %zu, chunk_pos: (%d, %d, %d)",
        daic_id, bucket_id, last_daic_id, bucket_id_of_last_daic, chunk_pos.x, chunk_pos.y, chunk_pos.z);

    if (m_chunk_metadata.active_daics.empty() || last_daic_id < daic_id) {
        LOG_F(ERROR, "An attempt to delete element %d was made, but DAIC size is %d", daic_id, last_daic_id);
        return;
    }

    m_chunk_metadata.active_chunk_info.chunk_pos[daic_id] = m_chunk_metadata.active_chunk_info.chunk_pos[last_daic_id];
    m_chunk_metadata.active_chunks_lod.chunks_lod[daic_id] = m_chunk_metadata.active_chunks_lod.chunks_lod[last_daic_id];
    std::swap(m_chunk_metadata.active_daics[daic_id], m_chunk_metadata.active_daics[last_daic_id]);
    m_bucket_id_to_daic_id[bucket_id_of_last_daic] = daic_id;

    chunk_buckets[bucket_id]._is_free = true;
    m_chunk_pos_to_bucket_id.erase(chunk_pos);
    m_chunk_metadata.active_daics.pop_back();
}

MeshBucket* VertexPool::getFirstFreeBucket()
{
    for (int i = 0; i < PoolConst::MAX_DAIC_AMOUNT; i++)
    {
        if (chunk_buckets[i]._is_free) {
            return &chunk_buckets[i];
        }
    }
    return nullptr;
}

void VertexPool::waitBuffer()
{
    if (m_sync)
    {
        while (1)
        {
            GLenum waitReturn = glClientWaitSync(m_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
            if (waitReturn == GL_ALREADY_SIGNALED || waitReturn == GL_CONDITION_SATISFIED) {
                return;
            }
        }
    }
}

void VertexPool::lockBuffer()
{
    if (m_sync) {
        glDeleteSync(m_sync);
    }

    m_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

VertexPool::~VertexPool()
{
    delete m_mesh_persistent_buffer;
}

void VertexPool::initBuckets()
{
    for (size_t bucket_idx = 0; bucket_idx < PoolConst::MAX_DAIC_AMOUNT; bucket_idx++)
    {
        chunk_buckets[bucket_idx]._start_ptr = m_mesh_persistent_buffer + (PoolConst::MAX_VERTICES_IN_BUCKET * bucket_idx);
        chunk_buckets[bucket_idx]._start_offset = PoolConst::MAX_VERTICES_IN_BUCKET * bucket_idx;
        chunk_buckets[bucket_idx]._id = bucket_idx;
        chunk_buckets[bucket_idx]._is_free = true;
    }
}

void VertexPool::formatVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribIFormat(0, 1, GL_UNSIGNED_INT, 0);
    glVertexAttribIFormat(1, 1, GL_UNSIGNED_INT, 0);

    glVertexBindingDivisor(0, 0);
    glVertexBindingDivisor(1, 0);

    glVertexAttribBinding(0, 0);
    glVertexAttribBinding(1, 1);

    glBindVertexBuffer(0, m_vbo, offsetof(Vertex, _xyzs), sizeof(Vertex));
    glBindVertexBuffer(1, m_vbo, offsetof(Vertex, _uvw), sizeof(Vertex));
}

void VertexPool::draw()
{
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);

    glMultiDrawArraysIndirect(
        GL_TRIANGLES,
        (GLvoid*)0,                                   // start with first draw command
        m_chunk_metadata.active_daics.size(),     // DAIC amount
        0                                             // tightly packed
    );
}

void VertexPool::createMeshBuffer() {
    LOG_F(INFO, "%d", glGetError());
    GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    size_t buffer_size = PoolConst::MAX_DAIC_AMOUNT * PoolConst::MAX_VERTICES_IN_BUCKET * sizeof(Vertex);
    LOG_F(INFO, "createMeshBuffer buffer_size: %zu, daic size: %d", buffer_size, m_chunk_metadata.active_daics.size());
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferStorage(GL_ARRAY_BUFFER, buffer_size, 0, flags);
    m_mesh_persistent_buffer = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, flags);

    if (m_mesh_persistent_buffer == nullptr) {
        LOG_F(INFO, "%d", glGetError());
    }

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, m_chunk_metadata.active_daics.size() * sizeof(DAIC), NULL, GL_DYNAMIC_DRAW);
}

void VertexPool::updateMeshBuffer(std::vector<Vertex>& mesh, int buffer_offset)
{
    OPTICK_EVENT("updateMeshBuffer");
    waitBuffer();
    std::copy(mesh.begin(), mesh.end(), m_mesh_persistent_buffer + buffer_offset);
    lockBuffer();
    
    //int index = 0;
    //for (DAIC daic : m_chunk_metadata.active_daics) {
    //    LOG_F(INFO, "DAIC: %d, count: %d, first: %d", index, daic.count, daic.first);
    //    index++;
    //}

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, m_chunk_metadata.active_daics.size() * sizeof(DAIC), m_chunk_metadata.active_daics.data(), GL_DYNAMIC_DRAW);
}

void VertexPool::createChunkInfoBuffer()
{
    OPTICK_EVENT("createChunkInfoBuffer");
    m_chunk_info_ssbo = 0;
    glGenBuffers(1, &m_chunk_info_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_chunk_info_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(m_chunk_metadata.active_chunk_info), &m_chunk_metadata.active_chunk_info, GL_STATIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_chunk_info_ssbo);
}

size_t VertexPool::getBucketIdFromStartOffset(size_t offset)
{
    return offset / PoolConst::MAX_ADDED_VERTICES;
}

void VertexPool::createChunkLodBuffer()
{
    OPTICK_EVENT("createChunkLodBuffer");
    m_chunks_lod_ssbo = 1;
    glGenBuffers(1, &m_chunks_lod_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_chunks_lod_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(m_chunk_metadata.active_chunks_lod), &m_chunk_metadata.active_chunks_lod, GL_STATIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_chunks_lod_ssbo);
}


