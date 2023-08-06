#pragma once
#include <unordered_map>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <cmath>
#include <optional>
#include <unordered_set>
#include "ChunksMap.h"
#include "../terrain_generation/TerrainGenerator.h"
#include "../shader/Shader.h"
#include "../TextureManager.h"
#include "../block/Block.h"
#include "../Camera.h"

/*
				
Convention:
world_pos is what it seems to be
chunk_pos is position of a chunk in the world
block_pos is position of a block inside the chunk

*/

class Chunk;

class ChunkManager
{
public:
	ChunkManager(Camera& camera, TerrainGenerator world_generator, int render_distance_halved_xz = 4, int render_distance_height = 4);
	~ChunkManager() = default;
	void launchAddToChunksMapTask();
	void addToChunksMapTask();
	ChunksMap& getChunksMap();
	MeshState getMeshState(glm::ivec3 chunk_pos);
	TerrainGenerator& getTerrainGenerator();
	glm::vec3 getChunkPosition(glm::vec3 world_pos);
	glm::vec3 getChunkBlockPosition(glm::vec3 world_pos);
	std::atomic<bool>& getIsReadyToProcessChunks();
	void addToChunksMap();
	void deleteFromChunksMap();
	void tryAddChunk(glm::ivec3 chunk_pos);

private:
	 ChunksMap m_chunks_map;
	 std::atomic<bool> m_ready_to_process_chunks{ false };
	 Camera& m_camera;
	 TerrainGenerator m_terrain_generator;
	 int m_render_distance_xz_halved;
	 int m_render_distance_height;
};

