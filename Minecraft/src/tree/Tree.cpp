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
		glm::ivec3 chunk_pos = placeBlock(chunk, { block_pos.x, block_pos.y + i, block_pos.z }, Block::OAK_LOG);
		chunks_to_update.insert(chunk_pos);
	}
	return chunks_to_update;
}

std::unordered_set<glm::ivec3> Tree::addCrown(Chunk& chunk, glm::ivec3 block_pos)
{
	std::unordered_set<glm::ivec3> chunks_to_update = {};
	int crown_width_halved = CROWN_WIDTH / 2;
	int odd_remainder = CROWN_WIDTH % 2;
	for (uint8_t i = TRUNK_HEIGHT; i < TRUNK_HEIGHT + CROWN_HEIGHT; i++)
	{
		for (int j = -crown_width_halved; j < crown_width_halved + odd_remainder; j++)
		{
			for (int k = -crown_width_halved; k < crown_width_halved + odd_remainder; k++)
			{
				glm::ivec3 chunk_pos = placeBlock(chunk, { block_pos.x + j, block_pos.y + i, block_pos.z + k }, Block::OAK_LEAVES);
				chunks_to_update.insert(chunk_pos);
			}
		}
	}
	return chunks_to_update;
}

glm::ivec3 Tree::placeBlock(Chunk& chunk, glm::ivec3 block_pos, Block::block_id block_type)
{
	int x, y, z;

	x = block_pos.x;
	y = block_pos.y;
	z = block_pos.z;

	glm::ivec3 chunk_pos = chunk.getPos();

	chunk_pos.x += determineChunkOffset(x);
	chunk_pos.y += determineChunkOffset(y);
	chunk_pos.z += determineChunkOffset(z);

	Chunk& chunk_to_modify = chunk.getNeighborChunk(chunk_pos);

	x = determineBlockOffset(x);
	y = determineBlockOffset(y);
	z = determineBlockOffset(z);

	chunk_to_modify.setBlock({ x, y, z }, block_type);

	// TODO: This should be set once, after whole tree is added
	//chunk_to_modify.getMesh().setMeshState(MeshState::READY_TO_DECORATE);
	return chunk_pos;
}

int Tree::determineChunkOffset(int block_pos)
{
	if (block_pos < 0) return (block_pos / CHUNK_SIZE_X) - 1;
	return block_pos / CHUNK_SIZE_X;
}

int Tree::determineBlockOffset(int block_pos)
{
	if (block_pos < 0) return CHUNK_SIZE_X + ((block_pos - 1) % CHUNK_SIZE_X);
	return block_pos % CHUNK_SIZE_X;
}
