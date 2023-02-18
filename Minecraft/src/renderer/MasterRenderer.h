#pragma once
#include <shared_mutex>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "ChunkRenderer.h"
#include "../Camera.h"

class MasterRenderer
{
public:
	MasterRenderer(ChunksMap& chunks_map, std::shared_mutex& chunks_map_mutex, std::condition_variable_any& should_process_chunks, std::atomic<bool>& is_ready_to_process_chunks);
	~MasterRenderer() = default;
	void initConfig() const;
	void clear() const;
	void render(Camera& camera);
	ChunkRenderer& getChunkRenderer();

private:
	ChunkRenderer m_chunk_renderer;
};

