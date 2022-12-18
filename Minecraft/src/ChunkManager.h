#pragma once
#include <unordered_map>
#include <cmath>
#include <optional>
#include <unordered_set>
#include "Chunk.h"
#include "Shader.h"
#include "TextureManager.h"
#include "Block.h"
#include "FastNoiseLite.h"
#include "WorldGenerator.h"
#include "Camera.h"
#include "optick.h"
#include "ChunksMap.h"

/*
				
Convention:
world_pos is what it seems to be
chunk_pos is position of a chunk in the world
block_pos is position of a block inside the chunk

*/

class ChunkManager
{
public:
	ChunkManager(Camera& camera, WorldGenerator world_generator);
	~ChunkManager() = default;
	void generateWorld();
	ChunksMap* getChunksMap();
	glm::vec3 getChunkPosition(glm::vec3 world_pos);
	glm::vec3 getChunkBlockPosition(glm::vec3 world_pos);
	Block::block_id getChunkBlockId(glm::vec3 world_pos);
	void updateBlock(glm::vec3 pos, Block::block_id block_id);
	void refreshChunks();
	void addChunk(glm::ivec3 chunk_pos);
	void deleteChunk(glm::ivec3 chunk_pos);

private:
	 ChunksMap m_chunks_map;
	 //TODO: pass this value in constructor 
	 int m_render_distance{ 16 };
	 Camera& m_camera;
	 WorldGenerator m_world_generator;
};

