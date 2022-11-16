#include "ChunkManager.h"

ChunkManager::ChunkManager(Shader shader)
	:
	m_shader{shader}
{
	generateWorld();
}

void ChunkManager::generateWorld()
{

	WorldGenerator world_generator;
	int seed = 1234;

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{

			glm::ivec3 chunk_pos(i, 0, j);
			std::unique_ptr<Chunk> current_chunk(new Chunk (&m_texture_manager, chunk_pos));
			height_map h_map = world_generator.generateChunkHeightMap(chunk_pos, seed);
			for (int x = 0; x < current_chunk->CHUNK_SIZE_X; x++)
			{
				for (int y = 0; y < current_chunk->CHUNK_SIZE_Y; y++)
				{
					for (int z = 0; z < current_chunk->CHUNK_SIZE_Z; z++)
					{
						glm::ivec3 current_chunk_pos{ x, y, z };
						if (y < h_map[x][z])
							current_chunk->setBlock(current_chunk_pos, Block::STONE);
						else
							current_chunk->setBlock(current_chunk_pos, Block::AIR);
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
	int x = floor(world_pos.x / Chunk::CHUNK_SIZE_X);
	int y = floor(world_pos.y / Chunk::CHUNK_SIZE_Y);
	int z = floor(world_pos.z / Chunk::CHUNK_SIZE_Z);

	return glm::ivec3(x, y, z);
}


glm::vec3 ChunkManager::getChunkBlockPosition(glm::vec3 world_pos)
{
	glm::ivec3 world_pos_int = world_pos;
	int M_X = Chunk::CHUNK_SIZE_X;
	int M_Y = Chunk::CHUNK_SIZE_Y;
	int M_Z = Chunk::CHUNK_SIZE_Z;

	
	// true modulo instead of C++ remainder modulo
	int x = ((world_pos_int.x % M_X) + M_X) % M_X;
	int y = ((world_pos_int.y % M_Y) + M_Y) % M_Y;
	int z = ((world_pos_int.z % M_Z) + M_Z) % M_Z;

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
