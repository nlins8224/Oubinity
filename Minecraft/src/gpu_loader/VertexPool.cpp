#include "VertexPool.h"

//void VertexPool::formatVBO()
//{
//    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
//
//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);
//    glEnableVertexAttribArray(2);
//    glEnableVertexAttribArray(3);
//
//    glVertexAttribFormat(0, 1, GL_UNSIGNED_INT, GL_FALSE, 0);
//    glVertexAttribFormat(1, 1, GL_UNSIGNED_INT, GL_FALSE, 0);
//    glVertexAttribFormat(2, 1, GL_UNSIGNED_INT, GL_FALSE, 0);
//    glVertexAttribFormat(3, 3, GL_UNSIGNED_INT, GL_FALSE, 0);
//
//    glVertexBindingDivisor(0, 0);
//    glVertexBindingDivisor(1, 0);
//    glVertexBindingDivisor(2, 0);
//    glVertexBindingDivisor(3, 0);
//
//    glVertexAttribBinding(0, 0);
//    glVertexAttribBinding(1, 1);
//    glVertexAttribBinding(2, 2);
//    glVertexAttribBinding(3, 3);
//
//    glBindVertexBuffer(0, m_vbo, offsetof(Vertex, xyzs), sizeof(Vertex));
//    glBindVertexBuffer(1, m_vbo, offsetof(Vertex, uvw), sizeof(Vertex));
//    glBindVertexBuffer(2, m_vbo, offsetof(Vertex, lod_scale), sizeof(Vertex));
//    glBindVertexBuffer(3, m_vbo, offsetof(Vertex, chunk_world_pos), sizeof(Vertex));
//}



void VertexPool::formatVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(2, 1, GL_FLOAT, GL_FALSE, 0);

    glVertexBindingDivisor(0, 0);
    glVertexBindingDivisor(1, 0);
    glVertexBindingDivisor(2, 0);

    glVertexAttribBinding(0, 0);
    glVertexAttribBinding(1, 1);
    glVertexAttribBinding(2, 2);

    glBindVertexBuffer(0, m_vbo, offsetof(Vertex, position), sizeof(Vertex));
    glBindVertexBuffer(1, m_vbo, offsetof(Vertex, texture), sizeof(Vertex));
    glBindVertexBuffer(2, m_vbo, offsetof(Vertex, shading), sizeof(Vertex));
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

void VertexPool::updateBuffer(std::vector<Vertex>& mesh, std::vector<DAIC>& daic)
{
    OPTICK_EVENT("updateBuffer");
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(Vertex), mesh.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_daicbo);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, daic.size() * sizeof(DAIC), daic.data(), GL_STATIC_DRAW);
}