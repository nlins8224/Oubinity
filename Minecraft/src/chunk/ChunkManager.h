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
	ChunkManager(Camera& camera, TerrainGenerator world_generator, int render_distance_halved_xz = 4, int render_distance_height = 6);
	~ChunkManager() = default;
	void launchAddToChunksMapTask();
	void addToChunksMapTask();
	ChunksMap& getChunksMap();
	TerrainGenerator& getTerrainGenerator();
	glm::vec3 getChunkPosition(glm::vec3 world_pos);
	glm::vec3 getChunkBlockPosition(glm::vec3 world_pos);
	Block::block_id getChunkBlockId(glm::vec3 world_pos);
	std::atomic<bool>& getIsReadyToProcessChunks();
	void updateBlock(glm::vec3 pos, Block::block_id block_id);
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

