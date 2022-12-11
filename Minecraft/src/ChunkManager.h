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

namespace std
{
	template<>
	struct hash<glm::ivec3>
	{
		const size_t operator()(const glm::ivec3& vec) const
		{
			size_t res = 17;
			res = res * 31 + std::hash<int>()(vec.x);
			res = res * 31 + std::hash<int>()(vec.y);
			res = res * 31 + std::hash<int>()(vec.z);
			return res;
		};
	};
}

using ChunksMap = std::unordered_map<glm::ivec3, Chunk, glm_ivec3_hasher>;

class ChunkManager
{
public:
	ChunkManager(Shader shader, Camera& camera, WorldGenerator world_generator);
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
	void renderChunks();
	void addTextures();
	void addChunkToLoadList(glm::ivec3 chunk_pos);
	void addChunkToUnloadList(glm::ivec3 chunk_pos);
	void loadAllChunksFromLoadList();
	void unloadAllChunksFromUnloadList();

private:
	 ChunksMap m_chunks;

	 std::unordered_set<glm::ivec3> m_chunks_to_load;
	 std::unordered_set<glm::ivec3> m_chunks_to_unload;
	 //TODO: pass those values in constructor 
	 int m_render_distance{ 16 };
	 int m_seed{ 1234 };
	 // TODO: ChunkManager should not have resposibility to know what Shaders are
	 Shader m_shader;
	 Camera& m_camera;
	 WorldGenerator m_world_generator;
	 // TODO: ChunkManager should not have responsibility to know what TextureManager is
	 TextureManager m_texture_manager{m_shader, 16, 16, 256 };
};

