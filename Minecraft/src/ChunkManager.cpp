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
			glm::ivec3 chunk_pos(i, 0, j);
			std::unique_ptr<Chunk> current_chunk(new Chunk (&m_texture_manager, chunk_pos));
			for (uint8_t x = 0; x < current_chunk->CHUNK_SIZE; x++)
			{
				for (uint8_t y = 0; y < current_chunk->CHUNK_SIZE; y++)
				{
					for (uint8_t z = 0; z < current_chunk->CHUNK_SIZE; z++)
					{
						glm::ivec3 current_chunk_pos{ x, y, z };
						float chance = (double)rand() / RAND_MAX;
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
							current_chunk->setBlock(current_chunk_pos, Block::block_id::COBBLESTONE);

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
	int x = static_cast<int>(floor(world_pos.x)) / Chunk::CHUNK_SIZE;
	int y = static_cast<int>(floor(world_pos.y)) / Chunk::CHUNK_SIZE;
	int z = static_cast<int>(floor(world_pos.z)) / Chunk::CHUNK_SIZE;

	return glm::vec3(x, y, z);
}


glm::vec3 ChunkManager::getChunkBlockPosition(glm::vec3 world_pos)
{
	glm::ivec3 world_pos_abs = glm::abs(world_pos);

	int x = static_cast<int>(floor(world_pos_abs.x)) % Chunk::CHUNK_SIZE;
	int y = static_cast<int>(floor(world_pos_abs.y)) % Chunk::CHUNK_SIZE;
	int z = static_cast<int>(floor(world_pos_abs.z)) % Chunk::CHUNK_SIZE;

	return glm::vec3(x, y, z);
}

void ChunkManager::updateBlock(glm::vec3 world_pos, Block::block_id type)
{
	glm::vec3 chunk_pos = getChunkPosition(world_pos);
	if (m_chunks.find(chunk_pos) == m_chunks.end())
		if (type == Block::AIR)
			return;
		else
		{
			std::unique_ptr<Chunk> chunk(new Chunk(&m_texture_manager, chunk_pos));
			m_chunks[chunk_pos] = *chunk;
		}

	std::cout << "Chunk found at: x " << chunk_pos.x << " y: " << chunk_pos.y << " z: " << chunk_pos.z << " " << std::endl;

	Chunk& chunk = m_chunks[chunk_pos];
	glm::ivec3 block_chunk_pos = getChunkBlockPosition(world_pos);

	if (chunk.getBlockId(block_chunk_pos) == type)
		return;

	std::cout << "HEY!!!" << std::endl;

	chunk.setBlock(block_chunk_pos, type);
	chunk.updateChunk();
}
