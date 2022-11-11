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
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			glm::ivec3 chunk_pos(i - 1, -1, j - 1);
			std::unique_ptr<Chunk> current_chunk(new Chunk (&m_texture_manager, chunk_pos));
			for (uint8_t x = 0; x < current_chunk->CHUNK_SIZE; x++)
			{
				for (uint8_t y = 0; y < current_chunk->CHUNK_SIZE; y++)
				{
					for (uint8_t z = 0; z < current_chunk->CHUNK_SIZE; z++)
					{
						glm::ivec3 current_chunk_pos{ x, y, z };
						current_chunk->setBlock(current_chunk_pos, Block::COBBLESTONE);
			/*			float chance = (double)rand() / RAND_MAX;
						if (y == 15)
						{
							if (chance < 0.80) current_chunk->setBlock(current_chunk_pos, Block::block_id::AIR);
							else if (chance < 0.95) current_chunk->setBlock(current_chunk_pos, Block::block_id::SAND);
							else current_chunk->setBlock(current_chunk_pos, Block::block_id::COBBLESTONE);
						}
						else if (y > 12)
						{
							if (chance < 0.50) current_chunk->setBlock(current_chunk_pos, Block::block_id::AIR);
							else if (chance < 0.75) current_chunk->setBlock(current_chunk_pos, Block::block_id::SAND);
							else current_chunk->setBlock(current_chunk_pos, Block::block_id::COBBLESTONE);
						}
						else
							current_chunk->setBlock(current_chunk_pos, Block::block_id::COBBLESTONE);*/

					}	
				}
			}

			m_chunks[chunk_pos] = *current_chunk;
			
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

glm::vec3 ChunkManager::getChunkPosition(glm::vec3 world_pos)
{
	int x = floor(world_pos.x / Chunk::CHUNK_SIZE);
	int y = floor(world_pos.y / Chunk::CHUNK_SIZE);
	int z = floor(world_pos.z / Chunk::CHUNK_SIZE);

	return glm::ivec3(x, y, z);
}


glm::vec3 ChunkManager::getChunkBlockPosition(glm::vec3 world_pos)
{
	glm::ivec3 world_pos_int = world_pos;
	uint8_t M = Chunk::CHUNK_SIZE;
	
	// true modulo instead of C++ remainder modulo
	int x = ((world_pos_int.x % M) + M) % M;
	int y = ((world_pos_int.y % M) + M) % M;
	int z = ((world_pos_int.z % M) + M) % M;

	return glm::ivec3(x, y, z);
}

Block::block_id ChunkManager::getChunkBlockId(glm::vec3 world_pos)
{
	glm::ivec3 chunk_pos = getChunkPosition(world_pos);
	bool is_in = m_chunks.count(chunk_pos);
	if (!is_in)
		return Block::NONE;

	glm::ivec3 block_pos = getChunkBlockPosition(world_pos);
	return m_chunks.at(chunk_pos).getBlockId(block_pos);
}

void ChunkManager::updateBlock(glm::vec3 world_pos, Block::block_id type)
{
	glm::vec3 chunk_pos = getChunkPosition(world_pos);
	if (m_chunks.find(chunk_pos) == m_chunks.end())
	{
		std::unique_ptr<Chunk> chunk{ new Chunk(&m_texture_manager, chunk_pos) };
		m_chunks[chunk_pos] = *chunk;
	}
		
	Chunk& chunk = m_chunks.at(chunk_pos);
	glm::ivec3 chunk_block_pos = getChunkBlockPosition(world_pos);

	if (chunk.getBlockId(chunk_block_pos) == type)
		return;
	
	chunk.setBlock(chunk_block_pos, type);
	chunk.updateChunk();
}
