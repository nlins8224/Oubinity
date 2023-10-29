#pragma once
#include <map>
#include <vector>
#include <queue>
#include <atomic>
#include "Renderer.h"
#include "ChunkRendererSettings.h"
#include "../terrain_generation/TerrainGenerator.h"
#include "../gpu_loader/ZoneVertexPool.h"
#include "../Camera.h"
#include "../shader/ChunkShader.h"
#include "../chunk/ChunksMap.h"
#include "../chunk/ChunkSize.h"
#include "../chunk/Chunk.h"
#include "../frustum/AABox.h"

#include "../third_party/BS_thread_pool.hpp"
#include "optick.h"
#include "../loguru.hpp"

class ChunkRenderer : public Renderer
{
public:
	ChunkRenderer() = delete;
	ChunkRenderer(Shader shader, Camera& camera, GLuint texture_array);
	void render(Camera& camera) override;

	void traverseScene();
	void updateBufferIfNeedsUpdate();
	void runTraverseSceneInDetachedThread();
	void drawChunksSceneMesh();
private:
	bool createInRenderDistanceChunks(); // called when scene was already traversed
	bool createChunkIfNotPresent(glm::ivec3 chunk_pos);
	void createChunk(glm::ivec3 chunk_pos);
	void createChunkTask(Chunk& chunk);
	bool deleteOutOfRenderDistanceChunks(); // called when scene was already traversed
	bool deleteChunkIfPresent(glm::ivec3 chunk_pos);
	void deleteChunk(glm::ivec3 chunk_pos);
	bool checkIfChunkLodNeedsUpdate(glm::ivec3 chunk_pos);

	Camera& m_camera;
	GLuint m_texture_array;

	std::unordered_map<glm::ivec3, Chunk> m_chunks_by_coord;
	std::queue<glm::ivec3> m_chunks_to_create;
	std::queue<glm::ivec3> m_chunks_to_delete;

	ZonePool::ZoneVertexPool* m_vertexpool;
	TerrainGenerator* m_terrain_generator;

	std::atomic<bool> m_buffer_needs_update;
	BS::thread_pool m_thread_pool;
};