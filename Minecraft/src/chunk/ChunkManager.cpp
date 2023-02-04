#include "ChunkManager.h"

ChunkManager::ChunkManager(Camera& camera, WorldGenerator world_generator, int render_distance)
	:
	m_camera{camera},
	m_world_generator{world_generator},
	m_render_distance{render_distance}
{
	generateWorld();
}

//TODO: Refactor
void ChunkManager::updateChunksMap()
{
	int player_chunk_pos_x = m_camera.getCameraPos().x / CHUNK_SIZE_X;
	int player_chunk_pos_z = m_camera.getCameraPos().z / CHUNK_SIZE_Z;

	for (auto& it : m_chunks_map)
	{
		Chunk& chunk = it.second;

		int chunk_pos_x = chunk.getPosition().x;
		int chunk_pos_z = chunk.getPosition().z;
		int chunk_pos_y = chunk.getPosition().y;

		if (player_chunk_pos_x != chunk_pos_x && player_chunk_pos_z != chunk_pos_z)
			continue;

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
			tryDeleteChunk(chunk.getPosition());
		}
		else
		{
			tryAddChunk({ max_x, chunk_pos_y, chunk_pos_z });
			tryAddChunk({ min_x, chunk_pos_y, chunk_pos_z });
			tryAddChunk({ chunk_pos_x, chunk_pos_y, max_z });
			tryAddChunk({ chunk_pos_x, chunk_pos_y, min_z });
		}
	}
}

void ChunkManager::tryAddChunk(glm::ivec3 chunk_pos)
{
	if (m_chunks_map.find(chunk_pos) != m_chunks_map.end())
		return;

	std::unique_ptr<Chunk> chunk{ new Chunk(chunk_pos, this) };
	m_world_generator.generateChunkTerrain(*chunk, this->m_render_distance);
	m_chunks_map[chunk_pos] = *chunk;
}

void ChunkManager::tryDeleteChunk(glm::ivec3 chunk_pos)
{
	if (m_chunks_map.find(chunk_pos) != m_chunks_map.end())
		return;

	m_chunks_map.erase(chunk_pos);
}

ChunksMap& ChunkManager::getChunksMap()
{
	return m_chunks_map;
}

void ChunkManager::generateWorld()
{

	for (int x = -m_render_distance; x < m_render_distance; x++)
	{
		for (int y = -m_render_distance; y < m_render_distance; y++)
		{
			for (int z = -m_render_distance; z < m_render_distance; z++)
			{
				glm::ivec3 chunk_pos(x, y, z);
				tryAddChunk(chunk_pos);
			}	
		}
	}
}

glm::vec3 ChunkManager::getChunkPosition(glm::vec3 world_pos)
{
	int x = floor(world_pos.x / CHUNK_SIZE_X);
	int y = floor(world_pos.y / CHUNK_SIZE_Y);
	int z = floor(world_pos.z / CHUNK_SIZE_Z);

	return glm::ivec3(x, y, z);
}


glm::vec3 ChunkManager::getChunkBlockPosition(glm::vec3 world_pos)
{
	glm::ivec3 world_pos_int = world_pos;
	int M_X = CHUNK_SIZE_X;
	int M_Y = CHUNK_SIZE_Y;
	int M_Z = CHUNK_SIZE_Z;

	
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
	chunk.getMesh().setMeshState(MeshState::READY);
}
