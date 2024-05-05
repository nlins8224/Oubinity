#include "MasterRenderer.h"

MasterRenderer::MasterRenderer(TerrainGenerator& terrain_generator, Camera& camera, GLuint skybox_texture_id, GLuint texture_array_id, GLuint water_texture_id, float water_height, int water_area, GLuint cloud_noise_texture_id, glm::ivec2 resolution)
	: m_chunk_renderer{ChunkRenderer(terrain_generator, ChunkShader(), camera, texture_array_id) },
	m_skybox_renderer{SkyboxShader(), skybox_texture_id},
	m_water_renderer{WaterShader(), water_texture_id, water_height, water_area},
	m_gradient_renderer{GradientShader()},
	m_sky_renderer{SkyShader(), cloud_noise_texture_id, resolution}
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
	//m_chunk_renderer.traverseScene();
	// 
	m_chunk_renderer.updateBufferIfNeedsUpdate();
	m_chunk_renderer.drawChunksSceneMesh();
	m_water_renderer.render(camera);
	m_sky_renderer.setTimeElapsed(m_time_elapsed_sec);
	m_sky_renderer.setMousePos(m_mouse_pos);
	m_sky_renderer.render(camera);
	//m_gradient_renderer.render(camera);
}

void MasterRenderer::setTimeElapsed(float time_in_sec)
{
	m_time_elapsed_sec = time_in_sec;
}

void MasterRenderer::setMousePos(glm::vec2 mouse_pos)
{
	m_mouse_pos = mouse_pos;
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

WaterRenderer& MasterRenderer::getWaterRenderer()
{
	return m_water_renderer;
}

SkyRenderer& MasterRenderer::getSkyRenderer()
{
	return m_sky_renderer;
}
