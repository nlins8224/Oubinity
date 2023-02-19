#include "ChunkManager.h"

ChunkManager::ChunkManager(Camera& camera, TerrainGenerator world_generator, int render_distance)
	:
	m_camera{camera},
	m_world_generator{world_generator},
	m_render_distance_halved{render_distance}
{
}

void ChunkManager::launchHandleTasks()
{
	std::thread(&ChunkManager::handleTasks, this).detach();
}

//TODO: Fix active waiting
void ChunkManager::handleTasks()
{
	while (true)
	{
		if (m_ready_to_process_chunks.load())
			continue;

		addToChunksMap();
		deleteFromChunksMap();
		m_ready_to_process_chunks = true;
		m_should_process_chunks.notify_one();
	}	
}

void ChunkManager::addToChunksMap()
{
	OPTICK_EVENT();
	int player_chunk_pos_x = m_camera.getCameraPos().x / CHUNK_SIZE_X;
	int player_chunk_pos_z = m_camera.getCameraPos().z / CHUNK_SIZE_Z;

	int min_x = player_chunk_pos_x - m_render_distance_halved;
	int max_x = player_chunk_pos_x + m_render_distance_halved;

	int min_z = player_chunk_pos_z - m_render_distance_halved;
	int max_z = player_chunk_pos_z + m_render_distance_halved;

	for (int x = min_x; x < max_x; x++)
	{
		for (int z = min_z; z < max_z; z++)
		{
			for (int y = -m_render_distance_halved; y < m_render_distance_halved; y++)
			{
				tryAddChunk({ x, y, z });
			}
		}
	}
}

void ChunkManager::deleteFromChunksMap()
{
	int player_chunk_pos_x = m_camera.getCameraPos().x / CHUNK_SIZE_X;
	int player_chunk_pos_z = m_camera.getCameraPos().z / CHUNK_SIZE_Z;

	int min_x = player_chunk_pos_x - m_render_distance_halved;
	int max_x = player_chunk_pos_x + m_render_distance_halved;

	int min_z = player_chunk_pos_z - m_render_distance_halved;
	int max_z = player_chunk_pos_z + m_render_distance_halved;

	for (auto it = m_chunks_map.begin(); it != m_chunks_map.end();)
	{
		auto& chunk = it->second;

		int chunk_pos_x = chunk.getPosition().x;
		int chunk_pos_y = chunk.getPosition().y;
		int chunk_pos_z = chunk.getPosition().z;

		glm::ivec3 chunk_pos{ chunk_pos_x, chunk_pos_y, chunk_pos_z };

		if (
			chunk_pos_x < min_x ||
			chunk_pos_x > max_x ||
			chunk_pos_z < min_z ||
			chunk_pos_z > max_z
			)
		{
			std::lock_guard<std::shared_mutex> lock(m_chunks_map_mutex);
			it = m_chunks_map.erase(it);

		}
		else
		{
			++it;
		}
	}

}

void ChunkManager::tryAddChunk(glm::ivec3 chunk_pos)
{
	if (m_chunks_map.find(chunk_pos) != m_chunks_map.end())
		return;

	std::unique_ptr<Chunk> chunk{ new Chunk(chunk_pos, this) };
	m_world_generator.generateChunkTerrain(*chunk, this->m_render_distance_halved);

	std::lock_guard<std::shared_mutex> lock(m_chunks_map_mutex);
	m_chunks_map[chunk_pos] = *chunk;
	m_chunks_map[chunk_pos].getMesh().setMeshState(MeshState::READY);
}

ChunksMap& ChunkManager::getChunksMap()
{
	return m_chunks_map;
}

std::shared_mutex& ChunkManager::getChunksMapMutex()
{
	return m_chunks_map_mutex;
}

std::condition_variable_any& ChunkManager::getShouldProcessChunks()
{
	return m_should_process_chunks;
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

std::atomic<bool>& ChunkManager::getIsReadyToProcessChunks()
{
	return m_ready_to_process_chunks;
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
