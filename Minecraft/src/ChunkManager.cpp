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
	for (uint8_t i = 0; i < 4; i++)
	{
		for (uint8_t j = 0; j < 2; j++)
		{
			chunk_pos chunk_position(i, -1, j);
			std::unique_ptr<Chunk> current_chunk(new Chunk (&m_texture_manager, chunk_position));
			for (uint8_t x = 0; x < current_chunk->CHUNK_SIZE; x++)
			{
				for (uint8_t y = 0; y < current_chunk->CHUNK_SIZE; y++)
				{
					for (uint8_t z = 0; z < current_chunk->CHUNK_SIZE; z++)
					{
						current_chunk->setBlock(x, y, z, Block::block_id::SAND);
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

std::unordered_map<chunk_pos, Chunk, chunk_pos_hasher> ChunkManager::getChunks()
{
	return m_chunks;
}

TextureManager ChunkManager::getTextureManager()
{
	return this->m_texture_manager;
}

