#include "VertexPool.h"

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

VertexPool::VertexPool()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    formatVBO();
    glGenBuffers(1, &m_daicbo);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
}


void VertexPool::draw(GLsizei draw_count)
{
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);

    glMultiDrawArraysIndirect(
        GL_TRIANGLES,
        (GLvoid*)0,     // start with first draw command
        draw_count,     // DAIC amount
        0               // tightly packed
    );
}

void VertexPool::updateDrawBuffer(std::vector<Vertex>& mesh, std::vector<DAIC>& daic)
{
    OPTICK_EVENT("updateDrawBuffer");
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(Vertex), mesh.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, daic.size() * sizeof(DAIC), daic.data(), GL_STATIC_DRAW);
}

void VertexPool::createChunkInfoBuffer(ChunkInfo* chunk_info)
{
    OPTICK_EVENT("createChunkInfoBuffer");
    m_chunk_info_ssbo = 0;
    glGenBuffers(1, &m_chunk_info_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_chunk_info_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(*chunk_info), chunk_info, GL_STATIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_chunk_info_ssbo);
}

void VertexPool::updateChunkInfoBuffer(ChunkInfo chunk_info)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_chunk_info_ssbo);
    GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    memcpy(p, &chunk_info, sizeof(chunk_info));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void VertexPool::createChunkLodBuffer(ChunksLod* chunks_lod)
{
    OPTICK_EVENT("createChunkLodBuffer");
    m_chunks_lod_ssbo = 1;
    glGenBuffers(1, &m_chunks_lod_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_chunks_lod_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(*chunks_lod), chunks_lod, GL_STATIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_chunks_lod_ssbo);
}