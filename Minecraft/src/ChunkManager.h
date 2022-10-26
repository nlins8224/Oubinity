#pragma once
#include <unordered_map>
#include <cmath>
#include "Chunk.h"
#include "Shader.h"
#include "TextureManager.h"


class ChunkManager
{
public:
	ChunkManager(Shader shader);
	~ChunkManager() = default;
	void generateWorld();
	std::unordered_map<chunk_pos, Chunk, chunk_pos_hasher> getChunks();
	TextureManager getTextureManager();

private:
	 glm::vec3 getChunkPosition(glm::vec3 position);
	 glm::vec3 getLocalChunkPosition(glm::vec3 position);
	 std::unordered_map<chunk_pos, Chunk, chunk_pos_hasher> m_chunks;
	 Shader m_shader;
	 TextureManager m_texture_manager{m_shader, 16, 16, 256 };
};

