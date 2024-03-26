#include "Tree.h"

Tree::Tree(uint8_t trunk_height, uint8_t crown_height, uint8_t crown_width)
	: TRUNK_HEIGHT{ trunk_height },
	CROWN_HEIGHT{ crown_height },
	CROWN_WIDTH{ crown_width }
{

}

bool Tree::addTree(Chunk& chunk, glm::ivec3 block_pos)
{
	addTrunk(chunk, block_pos);
	addCrown(chunk, block_pos);

	return true;
}

void Tree::addTrunk(Chunk& chunk, glm::ivec3 block_pos)
{
	for (uint8_t i = 0; i < TRUNK_HEIGHT; i++)
	{
		placeBlock(chunk, { block_pos.x, block_pos.y + i, block_pos.z }, Block::OAK_LOG);
	}
}

void Tree::addCrown(Chunk& chunk, glm::ivec3 block_pos)
{
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

				placeBlock(chunk, { block_pos.x + x, block_pos.y + y, block_pos.z + z }, Block::OAK_LEAVES);
			}
		}
	}
}

// May overflow when near INT_MAX
// b parameter has to be positive
inline int roundDownDivide(int a, int b) {
	if (a >= 0) return a / b;
	else return (a - b + 1) / b;
}

// true modulo instead of C++ remainder modulo
inline int getMod(int pos, int mod)
{
	return ((pos % mod) + mod) % mod;
}

void Tree::placeBlock(Chunk& chunk, glm::ivec3 block_pos, Block::block_id block_type)
{
	int x, y, z;

	x = block_pos.x;
	y = block_pos.y;
	z = block_pos.z;

	glm::ivec3 chunk_pos = chunk.getPos();
	LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();

	int x_offset = roundDownDivide(x, lod.block_amount);
	int y_offset = roundDownDivide(y, lod.block_amount);
	int z_offset = roundDownDivide(z, lod.block_amount);

	if (x_offset == 0 && y_offset == 0 && z_offset == 0)
	{
		chunk.setBlock({ x, y, z }, block_type);	
	}
	else
	{
		chunk_pos.x += x_offset;
		chunk_pos.y += y_offset;
		chunk_pos.z += z_offset;

		ChunkNeighbors& chunk_neighbors = chunk.getNeighbors();
		if (chunk_neighbors.find(chunk_pos) != chunk_neighbors.end())
		{
			Chunk* chunk_to_modify = chunk.getNeighbors().at(chunk_pos);

			// Do not set trees across lod
			if (chunk_to_modify->getLevelOfDetail().level != chunk.getLevelOfDetail().level)
			{
				return;
			}

			x = getMod(x, lod.block_amount);
			y = getMod(y, lod.block_amount);
			z = getMod(z, lod.block_amount);

			chunk_to_modify->setBlock({ x, y, z }, block_type);
		}
	
	}
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