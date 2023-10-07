#include "MasterRenderer.h"

//TODO: DI instead of those arugments in constructor signature
MasterRenderer::MasterRenderer(Camera& camera, GLuint skybox_texture_id, GLuint texture_array_id)
	: m_chunk_renderer{ ChunkRenderer(ChunkShader(), camera, texture_array_id) },
	m_skybox_renderer{SkyboxShader(), skybox_texture_id},
	m_gradient_renderer{GradientShader()}
{
	
}

void MasterRenderer::initConfig()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void MasterRenderer::clear() const
{
	glClearColor(0.2f, 0.3f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MasterRenderer::render(Camera& camera)
{
	m_chunk_renderer.traverseScene();
	m_chunk_renderer.drawChunksSceneMesh();
	m_gradient_renderer.render(camera);
}

ChunkRenderer& MasterRenderer::getChunkRenderer()
{
	return m_chunk_renderer;
}

SkyboxRenderer& MasterRenderer::getSkyboxRenderer()
{
	return m_skybox_renderer;
}

GradientRenderer& MasterRenderer::getGradientRenderer()
{
	return m_gradient_renderer;
}
