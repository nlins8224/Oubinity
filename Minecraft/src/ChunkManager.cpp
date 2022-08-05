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
			chunk_pos chunk_position(i - 4, -1, j - 4);
			Chunk current_chunk(&m_texture_manager, chunk_position);
			for (int x = 0; x < current_chunk.CHUNK_SIZE; x++)
			{
				for (int y = 0; y < current_chunk.CHUNK_SIZE; y++)
				{
					for (int z = 0; z < current_chunk.CHUNK_SIZE; z++)
					{
						if (z == 0)
							current_chunk.setBlock(x, y, z, Block::block_id::DIRT);
						else 
							current_chunk.setBlock(x, y, z, Block::block_id::COBBLESTONE);
					}	
				}
			}

			m_chunks[chunk_position] = current_chunk;
			
		}
	}

	for (auto& chunk : m_chunks)
	{
		chunk.second.updateChunk();

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

