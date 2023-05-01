#include "ChunkManager.h"

ChunkManager::ChunkManager(Camera& camera, TerrainGenerator world_generator, int render_distance_halved_xz, int render_distance_height)
	:
	m_camera{camera},
	m_terrain_generator{world_generator},
	m_render_distance_xz_halved{ render_distance_halved_xz },
	m_render_distance_height{ render_distance_height }
{
}

void ChunkManager::launchAddToChunksMapTask()
{
	std::thread(&ChunkManager::addToChunksMapTask, this).detach();
}

void ChunkManager::addToChunksMapTask()
{
	while (true)
	{
		m_ready_to_process_chunks.wait(true);

		addToChunksMap();

		m_ready_to_process_chunks.store(true);
		m_ready_to_process_chunks.notify_one();
	}	
}

void ChunkManager::addToChunksMap()
{
	OPTICK_EVENT();
	int player_chunk_pos_x = m_camera.getCameraPos().x / CHUNK_SIZE_X;
	int player_chunk_pos_z = m_camera.getCameraPos().z / CHUNK_SIZE_Z;

	int min_x = player_chunk_pos_x - m_render_distance_xz_halved;
	int max_x = player_chunk_pos_x + m_render_distance_xz_halved;

	int min_z = player_chunk_pos_z - m_render_distance_xz_halved;
	int max_z = player_chunk_pos_z + m_render_distance_xz_halved;

	for (int x = max_x; x > min_x; x--)
	{
		for (int z = max_z; z > min_z; z--)
		{
			for (int y = m_render_distance_height - 1; y >= 0; y--)
			{
				tryAddChunk({ x, y, z });
				tryDecorateChunk({ x, y, z });
			}
		}
	}
}

void ChunkManager::deleteFromChunksMap()
{
	int player_chunk_pos_x = m_camera.getCameraPos().x / CHUNK_SIZE_X;
	int player_chunk_pos_z = m_camera.getCameraPos().z / CHUNK_SIZE_Z;

	int min_x = player_chunk_pos_x - m_render_distance_xz_halved;
	int max_x = player_chunk_pos_x + m_render_distance_xz_halved;

	int min_z = player_chunk_pos_z - m_render_distance_xz_halved;
	int max_z = player_chunk_pos_z + m_render_distance_xz_halved;

	for (auto it = m_chunks_map.begin(); it != m_chunks_map.end();)
	{
		auto& chunk = it->second;

		int chunk_pos_x = chunk.getPos().x;
		int chunk_pos_y = chunk.getPos().y;
		int chunk_pos_z = chunk.getPos().z;

		glm::ivec3 chunk_pos{ chunk_pos_x, chunk_pos_y, chunk_pos_z };

		if (
			chunk_pos_x < min_x ||
			chunk_pos_x > max_x ||
			chunk_pos_z < min_z ||
			chunk_pos_z > max_z
			)
		{
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
	m_terrain_generator.generateChunkTerrain(*chunk);

	m_chunks_map[chunk_pos] = *chunk;
	m_chunks_map[chunk_pos].getMesh().setMeshState(MeshState::READY);
	std::cout << "READY!" << std::endl;
}

void ChunkManager::tryDecorateChunk(glm::ivec3 chunk_pos)
{
	if (m_chunks_map.find(chunk_pos) == m_chunks_map.end())
		return;

	if (getMeshState(chunk_pos) != MeshState::READY_TO_DECORATE)
		return;

	if (!getAllNeighborChunksStateGreaterOrEqualTo(chunk_pos, MeshState::READY_TO_DECORATE))
		return;

	m_terrain_generator.decorateChunkTerrain(m_chunks_map[chunk_pos]);
	m_chunks_map[chunk_pos].getMesh().setMeshState(MeshState::DECORATED);
	std::cout << "DECORATED!" << std::endl;
}

ChunksMap& ChunkManager::getChunksMap()
{
	return m_chunks_map;
}

TerrainGenerator& ChunkManager::getTerrainGenerator()
{
	return m_terrain_generator;
}

glm::vec3 ChunkManager::getChunkPosition(glm::vec3 world_pos)
{
	int x = floor(world_pos.x / CHUNK_SIZE_X);
	int y = floor(world_pos.y / CHUNK_SIZE_Y);
	int z = floor(world_pos.z / CHUNK_SIZE_Z);

	return glm::ivec3(x, y, z);
}

bool ChunkManager::getAllNeighborChunksStateEqualTo(glm::ivec3 chunk_pos, MeshState state)
{
	int x = chunk_pos.x;
	int y = chunk_pos.y;
	int z = chunk_pos.z;

	return
		getMeshState({ x - 1, y, z - 1 }) == state &&
		getMeshState({ x - 1, y, z })	  == state &&
		getMeshState({ x - 1, y, z + 1 }) == state &&
		getMeshState({ x, y, z - 1 })     == state &&
		getMeshState({ x, y, z + 1 })     == state &&
		getMeshState({ x + 1, y, z - 1 }) == state &&
		getMeshState({ x + 1, y, z })     == state &&
		getMeshState({ x + 1, y, z + 1 }) == state;	
}

bool ChunkManager::getAllNeighborChunksStateGreaterOrEqualTo(glm::ivec3 chunk_pos, MeshState state)
{
	int x = chunk_pos.x;
	int y = chunk_pos.y;
	int z = chunk_pos.z;

	return
		getMeshState({ x - 1, y, z - 1 }) >= state &&
		getMeshState({ x - 1, y, z })     >= state &&
		getMeshState({ x - 1, y, z + 1 }) >= state &&
		getMeshState({ x, y, z - 1 })     >= state &&
		getMeshState({ x, y, z + 1 })     >= state &&
		getMeshState({ x + 1, y, z - 1 }) >= state &&
		getMeshState({ x + 1, y, z })     >= state &&
		getMeshState({ x + 1, y, z + 1 }) >= state;
}

MeshState ChunkManager::getMeshState(glm::ivec3 chunk_pos)
{
	if (!m_chunks_map.count(chunk_pos))
		return MeshState::NONE;

	return m_chunks_map.at(chunk_pos).getMesh().getMeshState();
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
