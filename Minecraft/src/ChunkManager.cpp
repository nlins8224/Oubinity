#include "ChunkManager.h"

ChunkManager::ChunkManager(Camera& camera, WorldGenerator world_generator)
	:
	m_camera{camera},
	m_world_generator{world_generator}
{
	generateWorld();
}

void ChunkManager::refreshChunks()
{
	OPTICK_EVENT();
	for (auto& it : m_chunks_map)
	{
		Chunk& chunk = it.second;

		int player_chunk_pos_x = m_camera.getCameraPos().x / Chunk::CHUNK_SIZE_X;
		int player_chunk_pos_z = m_camera.getCameraPos().z / Chunk::CHUNK_SIZE_Z;

		int chunk_pos_x = chunk.getPosition().x;
		int chunk_pos_z = chunk.getPosition().z;

		int min_x = player_chunk_pos_x - m_render_distance;
		int max_x = player_chunk_pos_x + m_render_distance;

		int min_z = player_chunk_pos_z - m_render_distance;
		int max_z = player_chunk_pos_z + m_render_distance;

		if (
			chunk_pos_x < min_x ||
			chunk_pos_x > max_x ||
			chunk_pos_z < min_z ||
			chunk_pos_z > max_z
			)
		{
			deleteChunk(chunk.getPosition());
		}
		else
		{
			addChunk({ player_chunk_pos_x + m_render_distance, 0, chunk_pos_z });
			addChunk({ player_chunk_pos_x - m_render_distance, 0, chunk_pos_z });
			addChunk({ chunk_pos_x, 0, player_chunk_pos_z + m_render_distance });
			addChunk({ chunk_pos_x, 0, player_chunk_pos_z - m_render_distance });
		}
	}
}

void ChunkManager::addChunk(glm::ivec3 chunk_pos)
{
	OPTICK_EVENT();
	if (m_chunks_map.find(chunk_pos) != m_chunks_map.end())
		return;

	std::unique_ptr<Chunk> chunk{ new Chunk(chunk_pos, this) };
	m_world_generator.generateChunkTerrain(*chunk);
	m_chunks_map[chunk_pos] = *chunk;
}

void ChunkManager::deleteChunk(glm::ivec3 chunk_pos)
{
	OPTICK_EVENT();
	if (m_chunks_map.find(chunk_pos) != m_chunks_map.end())
		return;

	m_chunks_map.erase(chunk_pos);
}

ChunksMap* ChunkManager::getChunksMap()
{
	OPTICK_EVENT();
	return &m_chunks_map;
}

//TODO: Move this to WorldGenerator | is that chunk manager responsibility?
void ChunkManager::generateWorld()
{
	OPTICK_EVENT();

	for (int i = -m_render_distance; i < m_render_distance; i++)
	{
		for (int j = -m_render_distance; j < m_render_distance; j++)
		{
			glm::ivec3 chunk_pos(i, 0, j);
			addChunk(chunk_pos);
		}
	}
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
	bool is_in = m_chunks_map.count(chunk_pos);
	if (!is_in)
		return Block::AIR;

	glm::ivec3 block_pos = getChunkBlockPosition(world_pos);
	return m_chunks_map.at(chunk_pos).getBlockId(block_pos);
}

void ChunkManager::updateBlock(glm::vec3 world_pos, Block::block_id type)
{
	OPTICK_EVENT();
	glm::vec3 chunk_pos = getChunkPosition(world_pos);
	if (m_chunks_map.find(chunk_pos) == m_chunks_map.end())
	{
		std::unique_ptr<Chunk> chunk{ new Chunk(chunk_pos, this) };
		m_chunks_map[chunk_pos] = *chunk;
	}
		
	Chunk& chunk = m_chunks_map.at(chunk_pos);
	glm::ivec3 chunk_block_pos = getChunkBlockPosition(world_pos);

	if (chunk.getBlockId(chunk_block_pos) == type)
		return;
	
	chunk.setBlock(chunk_block_pos, type);
	chunk.setIsMeshLoaded(false);
}
