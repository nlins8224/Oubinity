#include "VertexPool.h"

void VertexPool::allocate(Chunk& chunk)
{
    LOG_F(INFO, "allocate");
    unsigned int added_faces = chunk.getAddedFacesAmount();
    if (added_faces == 0)
    {
        LOG_F(INFO, "Empty chunk. No faces added");
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
    m_stats.max_vertices_occurred = std::max(m_stats.max_vertices_occurred, (int)(6 * added_faces));
    LOG_F(INFO, "Added vertices: %d", added_faces * 6);
    LOG_F(INFO, "Added vertices offset: %d", first_free_bucket->_start_offset);
    LOG_F(INFO, "Max vertices occurred: %d", m_stats.max_vertices_occurred);

    LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
    glm::ivec3 chunk_pos = chunk.getPos();

    std::vector<Vertex> mesh{ chunk.getMesh().getMeshDataCopy() };
    size_t id = first_free_bucket->_id;
    size_t occupied_space = mesh.size() * sizeof(Vertex);
    updateMeshBuffer(mesh, first_free_bucket->_start_offset);
    first_free_bucket->_occupied_space = occupied_space;
    first_free_bucket->_is_free = false;

    m_active_daics[id] = daic;
    m_active_chunks_info.chunk_pos[id] = { chunk.getWorldPos(), id };
    m_active_chunks_lod.chunks_lod[id] = static_cast<GLuint>(lod.block_size);

}

//TODO
void VertexPool::free(size_t bucket_id, glm::ivec3 chunk_pos)
{
    m_chunk_buckets[bucket_id]._is_free = true;
    m_active_daics.erase(m_active_daics.begin() + bucket_id);
}

MeshBucket* VertexPool::getFirstFreeBucket()
{
    for (int i = 0; i < BUCKETS_AMOUNT; i++)
    {
        if (m_chunk_buckets[i]._is_free) {
            return &m_chunk_buckets[i];
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

VertexPool::VertexPool() :
    m_mesh_persistent_buffer{nullptr},
    m_active_daics(MAX_DAIC_AMOUNT)
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
}

VertexPool::~VertexPool()
{
    delete m_mesh_persistent_buffer;
}

void VertexPool::initBuckets()
{
    for (size_t bucket_idx = 0; bucket_idx < BUCKETS_AMOUNT; bucket_idx++)
    {
        m_chunk_buckets[bucket_idx]._start_ptr = m_mesh_persistent_buffer + (MAX_VERTICES_IN_BUCKET * bucket_idx);
        m_chunk_buckets[bucket_idx]._start_offset = MAX_VERTICES_IN_BUCKET * bucket_idx;
        m_chunk_buckets[bucket_idx]._id = bucket_idx;
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
        (GLvoid*)0,                // start with first draw command
        m_active_daics.size(),     // DAIC amount
        0                          // tightly packed
    );
}

void VertexPool::createMeshBuffer() {
    LOG_F(INFO, "%d", glGetError());
    GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    size_t buffer_size = BUCKETS_AMOUNT * MAX_VERTICES_IN_BUCKET * sizeof(Vertex);
    LOG_F(INFO, "createMeshBuffer buffer_size: %ld", buffer_size);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferStorage(GL_ARRAY_BUFFER, buffer_size, 0, flags);
    m_mesh_persistent_buffer = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, flags);

    if (m_mesh_persistent_buffer == nullptr) {
        LOG_F(INFO, "%d", glGetError());
    }

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, MAX_DAIC_AMOUNT * sizeof(DAIC), NULL, GL_STATIC_DRAW);
}

void VertexPool::updateMeshBuffer(std::vector<Vertex>& mesh, int buffer_offset)
{
    LOG_F(INFO, "updateMeshBuffer");
    OPTICK_EVENT("updateMeshBuffer");
    waitBuffer();
    std::copy(mesh.begin(), mesh.end(), m_mesh_persistent_buffer + buffer_offset);
    lockBuffer();

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, m_active_daics.size() * sizeof(DAIC), m_active_daics.data(), GL_STATIC_DRAW);
}

void VertexPool::createChunkInfoBuffer()
{
    OPTICK_EVENT("createChunkInfoBuffer");
    m_chunk_info_ssbo = 0;
    glGenBuffers(1, &m_chunk_info_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_chunk_info_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(m_active_chunks_info), &m_active_chunks_info, GL_STATIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_chunk_info_ssbo);
}

void VertexPool::createChunkLodBuffer()
{
    OPTICK_EVENT("createChunkLodBuffer");
    m_chunks_lod_ssbo = 1;
    glGenBuffers(1, &m_chunks_lod_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_chunks_lod_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(m_active_chunks_lod), &m_active_chunks_lod, GL_STATIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_chunks_lod_ssbo);
}


