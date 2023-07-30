#pragma once
#include <shared_mutex>
#include <condition_variable>
#include "Renderer.h"
#include "../gpu_loader/VertexPool.h"
#include "../Camera.h"
#include "../shader/ChunkShader.h"
#include "../chunk/ChunksMap.h"
#include "../chunk/ChunkSize.h"
#include "../chunk/Chunk.h"
#include "../frustum/AABox.h"

class ChunkRenderer : public Renderer
{
public:
	ChunkRenderer() = delete;
	ChunkRenderer(Shader shader, ChunksMap& chunks_map, std::atomic<bool>& is_ready_to_process_chunks, GLuint texture_array);
	void launchChunkProcessingTask();
	void render(Camera& camera) override;
	void processChunksMeshTask();
private:
	void processChunkMesh(Chunk& chunk);
	void loadWorldMesh();
	void drawWorldMesh();
	ChunksMap& m_chunks_map;
	std::atomic<bool>& m_is_ready_to_process_chunks;
	GLuint m_texture_array;
	Mesh m_world_mesh;
	std::vector<DAIC> m_world_mesh_daic;
	VertexPool* m_vertexpool;
	int m_daic_offset_counter;
	bool buffer_loaded{ false };
};