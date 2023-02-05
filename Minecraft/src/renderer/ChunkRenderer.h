#pragma once
#include <shared_mutex>
#include <condition_variable>
#include "Renderer.h"
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
	ChunkRenderer(Shader shader, ChunksMap& chunks_map, std::shared_mutex& chunks_map_mutex, std::condition_variable_any& should_process_chunks, std::atomic<bool>& is_ready_to_process_chunks);
	~ChunkRenderer();
	void launchChunkProcessingTask();
	void render(Camera& camera) override;
	void processChunksMeshTask() const;
private:
	void draw(const Mesh& mesh) const override;
	void processChunkMesh(Chunk& chunk) const;
	void loadChunkMesh(Chunk& chunk) const;
	bool isInFrustum(Camera& camera, Chunk& chunk) const;
	void renderChunk(Camera& camera, Chunk& chunk) const;
	ChunksMap& m_chunks_map;
	std::shared_mutex& m_chunks_map_mutex;
	std::atomic<bool>& m_is_ready_to_process_chunks;
	std::condition_variable_any& m_should_process_chunks;
};