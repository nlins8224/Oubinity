#include "ChunkManager.h"

ChunkManager::ChunkManager(Shader shader): m_shader{shader}
{
	generateWorld();
}

ChunkManager::~ChunkManager()
{
}

// Temporary, just for tests
void ChunkManager::generateWorld()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			chunk_pos chunk_position(i - 4, -1, j - 4);
			Chunk current_chunk(chunk_position, m_shader);
			for (int x = 0; x < current_chunk.CHUNK_SIZE; x++)
			{
				for (int y = 0; y < current_chunk.CHUNK_SIZE; y++)
				{
					for (int z = 0; z < current_chunk.CHUNK_SIZE; z++)
					{
						current_chunk.setBlock(x, y, z, Block::block_id::DIRT);
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

