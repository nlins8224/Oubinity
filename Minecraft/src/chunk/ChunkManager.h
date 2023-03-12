#pragma once
#include <unordered_map>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <cmath>
#include <optional>
#include <unordered_set>
#include "Chunk.h"
#include "ChunksMap.h"
#include "../terrain_generation/TerrainGenerator.h"
#include "../shader/Shader.h"
#include "../TextureManager.h"
#include "../block/Block.h"
#include "../Camera.h"
#include "../third_party/FastNoiseLite.h"
#include "optick.h"

/*
				
Convention:
world_pos is what it seems to be
chunk_pos is position of a chunk in the world
block_pos is position of a block inside the chunk

*/

class ChunkManager
{
public:
	ChunkManager(Camera& camera, TerrainGenerator world_generator, int render_distance = 4);
	~ChunkManager() = default;
	void launchHandleTasks();
	void handleTasks();
	ChunksMap& getChunksMap();
	std::shared_mutex& getChunksMapMutex();
	std::condition_variable_any& getShouldProcessChunks();

	glm::vec3 getChunkPosition(glm::vec3 world_pos);
	glm::vec3 getChunkBlockPosition(glm::vec3 world_pos);
	Block::block_id getChunkBlockId(glm::vec3 world_pos);
	std::atomic<bool>& getIsReadyToProcessChunks();
	void updateBlock(glm::vec3 pos, Block::block_id block_id);
	void addToChunksMap();
	void deleteFromChunksMap();
	void tryAddChunk(glm::ivec3 chunk_pos);


private:
	 //TODO: Write ConcurrentChunksMap
	 ChunksMap m_chunks_map;
	 std::shared_mutex m_chunks_map_mutex;
	 std::atomic<bool> m_ready_to_process_chunks{ false };
	 std::condition_variable_any m_should_process_chunks;
	 Camera& m_camera;
	 TerrainGenerator m_world_generator;
	 int m_render_distance_halved;
};

