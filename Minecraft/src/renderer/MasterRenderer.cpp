#include "MasterRenderer.h"

MasterRenderer::MasterRenderer()
	: m_chunk_renderer{ ChunkShader() }
{
	
}

void MasterRenderer::initConfig() const
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void MasterRenderer::clear() const
{
	glClearColor(0.2f, 0.3f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MasterRenderer::render(Camera& camera, ChunksMap& chunks_map)
{
	m_chunk_renderer.setChunks(chunks_map);
	m_chunk_renderer.render(camera);
}

