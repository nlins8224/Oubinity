#pragma once
#include <unordered_map>
#include <cmath>
#include <optional>
#include <unordered_set>
#include "Chunk.h"
#include "ChunksMap.h"
#include "WorldGenerator.h"
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
	ChunkManager(Camera& camera, WorldGenerator world_generator, int render_distance = 16);
	~ChunkManager() = default;
	void generateWorld();
	ChunksMap* getChunksMap();
	glm::vec3 getChunkPosition(glm::vec3 world_pos);
	glm::vec3 getChunkBlockPosition(glm::vec3 world_pos);
	Block::block_id getChunkBlockId(glm::vec3 world_pos);
	void updateBlock(glm::vec3 pos, Block::block_id block_id);
	void updateChunksMap();
	void tryAddChunk(glm::ivec3 chunk_pos);
	void tryDeleteChunk(glm::ivec3 chunk_pos);

private:
	 ChunksMap m_chunks_map;
	 Camera& m_camera;
	 WorldGenerator m_world_generator;
	 int m_render_distance;
};

