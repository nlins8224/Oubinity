#include "Tree.h"

Tree::Tree(uint8_t trunk_height, uint8_t crown_height, uint8_t crown_width)
	: TRUNK_HEIGHT{trunk_height},
	CROWN_HEIGHT{crown_height},
	CROWN_WIDTH{crown_width}
{
	
}

bool Tree::addTree(Chunk& chunk, glm::ivec3 block_pos)
{
	std::unordered_set<glm::ivec3> chunks_to_update_trunk = addTrunk(chunk, block_pos);
	std::unordered_set<glm::ivec3> chunks_to_update_crown = addCrown(chunk, block_pos);

	std::unordered_set<glm::ivec3> chunks_to_update_total = {};
	chunks_to_update_total.insert(chunks_to_update_trunk.begin(), chunks_to_update_trunk.end());
	chunks_to_update_total.insert(chunks_to_update_crown.begin(), chunks_to_update_crown.end());

	for (glm::ivec3 chunk_pos : chunks_to_update_total)
	{
		chunk.getNeighborChunk(chunk_pos).getMesh().setMeshState(MeshState::READY_TO_DECORATE);
	}

	return true;
}

std::unordered_set<glm::ivec3> Tree::addTrunk(Chunk& chunk, glm::ivec3 block_pos)
{
	std::unordered_set<glm::ivec3> chunks_to_update = {};
	for (uint8_t i = 0; i < TRUNK_HEIGHT; i++)
	{
		placeBlock(chunk, { block_pos.x, block_pos.y + i, block_pos.z }, Block::OAK_LOG, chunks_to_update);
	}
	return chunks_to_update;
}

std::unordered_set<glm::ivec3> Tree::addCrown(Chunk& chunk, glm::ivec3 block_pos)
{
	std::unordered_set<glm::ivec3> chunks_to_update = {};
	int crown_width_halved = CROWN_WIDTH / 2;
	int odd_remainder = CROWN_WIDTH % 2;
	for (uint8_t y = TRUNK_HEIGHT; y < TRUNK_HEIGHT + CROWN_HEIGHT; y++)
	{
		for (int x = -crown_width_halved; x < crown_width_halved + odd_remainder; x++)
		{
			for (int z = -crown_width_halved; z < crown_width_halved + odd_remainder; z++)
			{
				if (shouldCutBlock(x, y, z))
					continue;

				placeBlock(chunk, { block_pos.x + x, block_pos.y + y, block_pos.z + z }, Block::OAK_LEAVES, chunks_to_update);				
			}
		}
	}
	return chunks_to_update;
}

void Tree::placeBlock(Chunk& chunk, glm::ivec3 block_pos, Block::block_id block_type, std::unordered_set<glm::ivec3>& chunks_to_update)
{
	int x, y, z;

	x = block_pos.x;
	y = block_pos.y;
	z = block_pos.z;

	glm::ivec3 chunk_pos = chunk.getPos();

	chunk_pos.x += determineChunkOffset(x);
	chunk_pos.y += determineChunkOffset(y);
	chunk_pos.z += determineChunkOffset(z);

	if (chunk.getChunksMap().find(chunk_pos) == chunk.getChunksMap().end())
		return;

	Chunk& chunk_to_modify = chunk.getNeighborChunk(chunk_pos);

	x = determineBlockOffset(x);
	y = determineBlockOffset(y);
	z = determineBlockOffset(z);

	chunk_to_modify.setBlock({ x, y, z }, block_type);
}

int Tree::determineChunkOffset(int block_pos)
{
	if (block_pos < 0) return (block_pos / CHUNK_SIZE) - 1;
	return block_pos / CHUNK_SIZE;
}

int Tree::determineBlockOffset(int block_pos)
{
	if (block_pos < 0) return CHUNK_SIZE + ((block_pos) % CHUNK_SIZE);
	return block_pos % CHUNK_SIZE;
}

// Some programmer art to make trees look better
bool Tree::shouldCutBlock(int x, int y, int z)
{
	int crown_width_halved = CROWN_WIDTH / 2;

	if ((x == -crown_width_halved && z == -crown_width_halved) ||
		(x == -crown_width_halved && z == crown_width_halved) ||
		(x == crown_width_halved && z == -crown_width_halved) ||
		(x == crown_width_halved && z == crown_width_halved))
	{
		return true;
	}

	if ((y == TRUNK_HEIGHT || y == TRUNK_HEIGHT + CROWN_HEIGHT - 1) &&
		((x == -crown_width_halved && z == -crown_width_halved + 1) ||
		(x == -crown_width_halved && z == crown_width_halved - 1) ||
		(x == -crown_width_halved + 1 && z == -crown_width_halved) || 
		(x == -crown_width_halved + 1 && z == crown_width_halved) ||
		(x == crown_width_halved - 1 && z == -crown_width_halved) ||
		(x == crown_width_halved - 1 && z == crown_width_halved) ||
		(x == crown_width_halved && z == -crown_width_halved + 1) ||
		(x == crown_width_halved && z == crown_width_halved - 1)))
	{
		return true;
	}

	return false;
}
