#include "ChunkManager.h"

ChunkManager::ChunkManager(Shader shader, Camera& camera)
	:
	m_shader{shader},
	m_camera{camera}
{
	generateWorld();
}

void ChunkManager::prepareChunksMesh()
{
	for (auto& chunk : m_chunks)
	{
		chunk.second.prepareChunkMesh();
	}
}

// This can be opitimised
// Split this into:
// 1. Add chunks to add-list
// 2. Add chunks to remove-list
// 3. Batch-add chunks
// 4. Batch-remove chunks

void ChunkManager::refreshChunks()
{
	OPTICK_EVENT();
	for (auto& it : m_chunks)
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
			addChunkToUnloadList(chunk.getPosition());
		}
		else
		{
			addChunkToLoadList({ player_chunk_pos_x + m_render_distance, 0, chunk_pos_z });
			addChunkToLoadList({ player_chunk_pos_x - m_render_distance, 0, chunk_pos_z });
			addChunkToLoadList({ chunk_pos_x, 0, player_chunk_pos_z + m_render_distance });
			addChunkToLoadList({ chunk_pos_x, 0, player_chunk_pos_z - m_render_distance });
		}
	}

	loadAllChunksFromLoadList();
	unloadAllChunksFromUnloadList();
}

// TODO: this should be deprecated soon
void ChunkManager::generateChunk(std::unique_ptr<Chunk>& chunk, int seed)
{
	OPTICK_EVENT();
	WorldGenerator world_generator;
	height_map h_map = world_generator.generateChunkHeightMap(chunk->getPosition(), seed);
	for (int x = 0; x < Chunk::CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < Chunk::CHUNK_SIZE_Y; y++)
		{
			for (int z = 0; z < Chunk::CHUNK_SIZE_Z; z++)
			{
				glm::ivec3 block_pos{ x, y, z };
				if (y == h_map[x][z])
					chunk->setBlock(block_pos, Block::GRASS);
				else if (y < h_map[x][z] && y > h_map[x][z] - 10)
					chunk->setBlock(block_pos, Block::DIRT);
				else if (y < h_map[x][z])
					chunk->setBlock(block_pos, Block::STONE);
				else
					chunk->setBlock(block_pos, Block::AIR);
			}
		}
	}
}

// TODO: this should be part of WorldGenerator
// TODO: this should be at the first stage of prepareChunkMesh? 
void ChunkManager::generateChunk(Chunk& chunk, int seed)
{
	OPTICK_EVENT();

	if (chunk.m_generated)
		return;

	WorldGenerator world_generator;
	height_map h_map = world_generator.generateChunkHeightMap(chunk.getPosition(), seed);
	for (int x = 0; x < Chunk::CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < Chunk::CHUNK_SIZE_Y; y++)
		{
			for (int z = 0; z < Chunk::CHUNK_SIZE_Z; z++)
			{
				glm::ivec3 block_pos{ x, y, z };
				if (y == h_map[x][z])
					chunk.setBlock(block_pos, Block::GRASS);
				else if (y < h_map[x][z] && y > h_map[x][z] - 10)
					chunk.setBlock(block_pos, Block::DIRT);
				else if (y < h_map[x][z])
					chunk.setBlock(block_pos, Block::STONE);
				else
					chunk.setBlock(block_pos, Block::AIR);
			}
		}
	}
	chunk.m_generated = true;
}

void ChunkManager::renderChunks()
{
	OPTICK_EVENT();
	for (auto& chunk : m_chunks)
	{
		chunk.second.renderChunk();
	}
}

void ChunkManager::addTextures()
{
	std::string texture_name;
	for (int i = Block::DIRT; i != Block::AMOUNT; i++)
	{
		Block::block_id id = static_cast<Block::block_id>(i);
		texture_name = Block::getBlockType(id).texture;
		m_texture_manager.addTexture(texture_name, id);
	}
}

void ChunkManager::addChunkToLoadList(glm::ivec3 chunk_pos)
{
	m_chunks_to_load.insert(chunk_pos);
}

void ChunkManager::addChunkToUnloadList(glm::ivec3 chunk_pos)
{
	m_chunks_to_unload.insert(chunk_pos);
}

void ChunkManager::loadAllChunksFromLoadList()
{
	for (auto it = m_chunks_to_load.begin(), end = m_chunks_to_load.end(); it != end;)
	{
		glm::ivec3 chunk_pos = *it;
		if (m_chunks.find(chunk_pos) == m_chunks.end())
		{
			std::unique_ptr<Chunk> chunk{ new Chunk(&m_texture_manager, chunk_pos, this) };
			m_chunks[chunk_pos] = *chunk;
		}
		m_chunks.at(chunk_pos).prepareChunkMesh();
		generateChunk(m_chunks.at(chunk_pos), m_seed);
		it = m_chunks_to_load.erase(it);
	}
	std::cout << "LOADED!" << std::endl;
}

// are chunks resources freed?
void ChunkManager::unloadAllChunksFromUnloadList()
{
	for (auto it = m_chunks_to_unload.begin(), end = m_chunks_to_unload.end(); it != end;)
	{
		glm::ivec3 chunk_pos = *it;
		m_chunks.erase(chunk_pos);
		it = m_chunks_to_unload.erase(it);
	}
	std::cout << "ERASED!" << std::endl;
}

void ChunkManager::generateWorld()
{
	OPTICK_EVENT();
	addTextures();
	WorldGenerator world_generator;

	for (int i = -m_render_distance; i < m_render_distance; i++)
	{
		for (int j = -m_render_distance; j < m_render_distance; j++)
		{
			glm::ivec3 chunk_pos(i, 0, j);
			std::unique_ptr<Chunk> current_chunk(new Chunk (&m_texture_manager, chunk_pos, this));
			generateChunk(current_chunk, m_seed);
			m_chunks[chunk_pos] = *current_chunk;		
		}
	}

	m_texture_manager.generateMipmap();
	prepareChunksMesh();
}

std::unordered_map<glm::ivec3, Chunk, glm_ivec3_hasher> ChunkManager::getChunks()
{
	OPTICK_EVENT();
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
		return Block::AIR;

	glm::ivec3 block_pos = getChunkBlockPosition(world_pos);
	return m_chunks.at(chunk_pos).getBlockId(block_pos);
}

void ChunkManager::updateBlock(glm::vec3 world_pos, Block::block_id type)
{
	OPTICK_EVENT();
	glm::vec3 chunk_pos = getChunkPosition(world_pos);
	if (m_chunks.find(chunk_pos) == m_chunks.end())
	{
		std::unique_ptr<Chunk> chunk{ new Chunk(&m_texture_manager, chunk_pos, this) };
		m_chunks[chunk_pos] = *chunk;
	}
		
	Chunk& chunk = m_chunks.at(chunk_pos);
	glm::ivec3 chunk_block_pos = getChunkBlockPosition(world_pos);

	if (chunk.getBlockId(chunk_block_pos) == type)
		return;
	
	chunk.setBlock(chunk_block_pos, type);
	chunk.prepareChunkMesh();
}

