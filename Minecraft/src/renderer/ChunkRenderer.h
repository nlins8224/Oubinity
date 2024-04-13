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
#include "../third_party/parallel_hashmap/phmap.h"
#include "optick.h"
#include "../loguru.hpp"

struct ChunkBorder
{
	int min_x;
	int max_x;
	int min_z;
	int max_z;
};

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
	void traverseSceneLoop();
private:
	void initChunks();
	bool createInRenderDistanceChunks(); // called when scene was already traversed
	bool createChunkIfNotPresent(glm::ivec3 chunk_pos);
	void createChunk(glm::ivec3 chunk_pos);
	bool decorateChunkIfPresent(glm::ivec3 chunk_pos);
	bool decorateChunks();
	bool meshChunks();
	bool meshChunk(glm::ivec3 chunk_pos);
	bool deleteOutOfRenderDistanceChunks(); // called when scene was already traversed
	bool deleteChunkIfPresent(glm::ivec3 chunk_pos);
	void deleteChunk(glm::ivec3 chunk_pos);
	bool checkIfChunkLodNeedsUpdate(glm::ivec3 chunk_pos);
	void iterateOverChunkBorderAndCreate(ChunkBorder chunk_border);
	void iterateOverChunkBorderAndDelete(ChunkBorder chunk_border);
	void iterateOverChunkBorderAndUpdateLod(ChunkBorder chunk_border);
	bool isChunkOutOfBorder(glm::ivec3 chunk_pos, ChunkBorder chunk_border);

	void allocateChunks();
	void allocateChunk();
	void freeChunks();
	void freeChunk();

	Camera& m_camera;
	glm::ivec3 m_camera_last_chunk_pos;
	GLuint m_texture_array;

	// Meshing is done on render thread, but allocate and free are
	// done on main thread, because of OpenGL context requirements
	std::atomic<bool> m_buffer_needs_update; 

	using pmap = phmap::parallel_flat_hash_map<glm::ivec3, Chunk*,
		phmap::priv::hash_default_hash<glm::ivec3>,
		phmap::priv::hash_default_eq<glm::ivec3>,
		std::allocator<std::pair<const glm::ivec3, Chunk*>>,
		4, // 2^N submaps
		std::mutex>;
	pmap m_chunks_by_coord; // used in thread safe manner, shared between render and main threads
	std::vector<glm::ivec3> m_border_chunks; // used only on render thread
	std::queue<glm::ivec3> m_chunks_to_create; // used only on render thread

	std::queue<glm::ivec3> m_chunks_to_decorate; // used only on render thread

	std::queue<glm::ivec3> m_chunks_to_mesh;
	std::queue<glm::ivec3> m_chunks_to_delete; // used only on render thread

	std::queue<glm::ivec3> m_chunks_to_allocate; // render thread writes, main thread reads
	std::queue<glm::ivec3> m_chunks_to_free; // render thread writes, main thread reads

	VertexPool::ZoneVertexPool* m_vertexpool; // called only on main thread
	TerrainGenerator* m_terrain_generator; // called only on render thread

};