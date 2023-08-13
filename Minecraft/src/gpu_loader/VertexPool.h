#pragma once
#include <glad/glad.h>
#include "../chunk/Vertex.h"
#include "../renderer/ChunkRendererSettings.h"
#include "DAIC.h"
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#include "optick.h"

constexpr int CHUNKS_AMOUNT = ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS;

struct ChunkInfo
{
	glm::vec4 chunk_pos[CHUNKS_AMOUNT];
};

class VertexPool
{
public:
	VertexPool();
	void draw(GLsizei draw_count);
	void updateDrawBuffer(std::vector<Vertex>& mesh, std::vector<DAIC>& daic);
	void createChunkInfoBuffer(ChunkInfo chunk_info);
	void updateChunkInfoBuffer(ChunkInfo chunk_info);
private:
	void formatVBO();

	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_daicbo;
	GLuint m_chunk_info_ssbo;
	size_t m_vertices_amount;
};