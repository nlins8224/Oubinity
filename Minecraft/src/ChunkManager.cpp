#include "ChunkManager.h"

ChunkManager::ChunkManager()
{
	generateWorld();
}

// Temporary, just for tests
void ChunkManager::generateWorld()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			glm::ivec3 chunk_position(i - 4, -1, j - 4);
			Chunk current_chunk(chunk_position);
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

			m_chunks[key(chunk_position)] = current_chunk;
		
			for (auto& chunk : m_chunks)
			{
				chunk.second.updateChunk();
			}
		}
	}
	

	
}

std::unordered_map<long, Chunk> ChunkManager::getChunks()
{
	return m_chunks;
}

