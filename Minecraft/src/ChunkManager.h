#pragma once
#include <unordered_map>
#include <cmath>
#include <optional>
#include <queue>
#include "Chunk.h"
#include "Shader.h"
#include "TextureManager.h"
#include "Block.h"
#include "FastNoiseLite.h"
#include "WorldGenerator.h"
#include "Camera.h"
#include "optick.h"

/*

Chunk possible states:

					TO RENDER
				  /
TO LOAD -> LOADED 
				  \
					TO UNLOAD -> (deleted)

				
Convention:
world_pos is what it seems to be
chunk_pos is position of a chunk in the world
block_pos is position of a block inside the chunk

*/

using ChunksMap = std::unordered_map<glm::ivec3, Chunk, glm_ivec3_hasher>;

class ChunkManager
{
public:
	ChunkManager(Shader shader, Camera& camera);
	~ChunkManager() = default;
	void generateWorld();
	std::unordered_map<glm::ivec3, Chunk, glm_ivec3_hasher> getChunks();
	TextureManager getTextureManager();
	glm::vec3 getChunkPosition(glm::vec3 world_pos);
	glm::vec3 getChunkBlockPosition(glm::vec3 world_pos);
	Block::block_id getChunkBlockId(glm::vec3 world_pos);
	void updateBlock(glm::vec3 pos, Block::block_id block_id);
	void prepareChunksMesh();
	void refreshChunks();
	void generateChunk(std::unique_ptr<Chunk>& chunk, int seed);
	void generateChunk(Chunk& chunk, int seed);
	void renderChunks();
	void addTextures();
	void addChunkToLoadQueue(glm::ivec3 chunk_pos);
	void addChunkToUnloadQueue(glm::ivec3 chunk_pos);
	void loadAllChunksFromLoadQueue();
	void unloadAllChunksFromUnloadQueue();

private:
	 ChunksMap m_chunks;

	 std::queue<glm::ivec3> m_chunks_to_load;
	 std::queue<glm::ivec3> m_chunks_to_unload;
	 //TODO: pass those values in constructor 
	 int m_render_distance{ 16 };
	 int m_seed{ 1234 };
	 Shader m_shader;
	 Camera& m_camera;
	 TextureManager m_texture_manager{m_shader, 16, 16, 256 };
};

