#pragma once
#include <unordered_map>
#include <cmath>
#include <optional>
#include "Chunk.h"
#include "Shader.h"
#include "TextureManager.h"
#include "Block.h"

/*
Convention:
world_pos is what it seems to be
chunk_pos is position of a chunk in the world
block_pos is position of a block inside the chunk
*/

class ChunkManager
{
public:
	ChunkManager(Shader shader);
	~ChunkManager() = default;
	void generateWorld();
	std::unordered_map<glm::ivec3, Chunk, glm_ivec3_hasher> getChunks();
	TextureManager getTextureManager();
	glm::vec3 getChunkPosition(glm::vec3 world_pos);
	glm::vec3 getChunkBlockPosition(glm::vec3 world_pos);
	Block::block_id getChunkBlockId(glm::vec3 world_pos);
	void updateBlock(glm::vec3 pos, Block::block_id block_id);

private:
	 std::unordered_map<glm::ivec3, Chunk, glm_ivec3_hasher> m_chunks;
	 Shader m_shader;
	 TextureManager m_texture_manager{m_shader, 16, 16, 256 };
};

