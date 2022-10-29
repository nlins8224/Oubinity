#include "ChunkManager.h"

ChunkManager::ChunkManager(Shader shader)
	:
	m_shader{shader}
{
	generateWorld();
}


// Temporary, just for tests
void ChunkManager::generateWorld()
{
	for (uint8_t i = 0; i < 8; i++)
	{
		for (uint8_t j = 0; j < 8; j++)
		{
			glm::ivec3 chunk_position(i, 0, j);
			std::unique_ptr<Chunk> current_chunk(new Chunk (&m_texture_manager, chunk_position));
			for (uint8_t x = 0; x < current_chunk->CHUNK_SIZE; x++)
			{
				for (uint8_t y = 0; y < current_chunk->CHUNK_SIZE; y++)
				{
					for (uint8_t z = 0; z < current_chunk->CHUNK_SIZE; z++)
					{
						glm::ivec3 pos{ x, y, z };
						float chance = (double)rand() / RAND_MAX;
						if (y == 15)
						{
							if (chance < 0.80) current_chunk->setBlock(pos, Block::block_id::AIR);
							else if (chance < 0.95) current_chunk->setBlock(pos, Block::block_id::SAND);
							else current_chunk->setBlock(pos, Block::block_id::COBBLESTONE);
						}
						else if (y > 12)
						{
							if (chance < 0.50) current_chunk->setBlock(pos, Block::block_id::AIR);
							else if (chance < 0.75) current_chunk->setBlock(pos, Block::block_id::SAND);
							else current_chunk->setBlock(pos, Block::block_id::COBBLESTONE);
						}
						else
							current_chunk->setBlock(pos, Block::block_id::COBBLESTONE);

					}	
				}
			}

			m_chunks[chunk_position] = *current_chunk;
			
		}
	}

	m_texture_manager.generateMipmap();

	for (auto& chunk : m_chunks)
	{
		chunk.second.prepareChunkMesh();
		chunk.second.loadChunkMesh();
	}
	
}

std::unordered_map<glm::ivec3, Chunk, glm_ivec3_hasher> ChunkManager::getChunks()
{
	return m_chunks;
}

TextureManager ChunkManager::getTextureManager()
{
	return this->m_texture_manager;
}

glm::vec3 ChunkManager::getChunkPosition(glm::vec3 position)
{
	int x = static_cast<int>(floor(position.x)) / Chunk::CHUNK_SIZE;
	int y = static_cast<int>(floor(position.y)) / Chunk::CHUNK_SIZE;
	int z = static_cast<int>(floor(position.z)) / Chunk::CHUNK_SIZE;

	return glm::vec3(x, y, z);
}

glm::vec3 ChunkManager::getChunkPosition(Chunk chunk)
{
	return chunk.getPosition();
}

glm::vec3 ChunkManager::getLocalChunkPosition(glm::vec3 position)
{
	int x = static_cast<int>(floor(position.x)) % Chunk::CHUNK_SIZE;
	int y = static_cast<int>(floor(position.y)) % Chunk::CHUNK_SIZE;
	int z = static_cast<int>(floor(position.z)) % Chunk::CHUNK_SIZE;

	return glm::vec3(x, y, z);
}
