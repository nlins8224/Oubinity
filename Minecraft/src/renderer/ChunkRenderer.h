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
	ChunkRenderer(Shader shader, ChunksMap& chunks_map, std::atomic<bool>& is_ready_to_process_chunks, GLuint texture_array);
	~ChunkRenderer();
	void launchChunkProcessingTask();
	void render(Camera& camera) override;
	void processChunksMeshTask() const;
private:
	void draw(const Mesh& mesh) const;
	void processChunkMesh(Chunk& chunk) const;
	void loadChunkMesh(Chunk& chunk) const;
	bool isInFrustum(Camera& camera, Chunk& chunk) const;
	void renderChunk(Camera& camera, Chunk& chunk) const;
	ChunksMap& m_chunks_map;
	std::atomic<bool>& m_is_ready_to_process_chunks;
	GLuint m_texture_array;
};