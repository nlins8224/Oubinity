#include "Tree.h"

Tree::Tree(uint8_t height)
	: TREE_HEIGHT{height}
{
	
}

bool Tree::addTree(Chunk& chunk, glm::ivec3 block_pos)
{
	int x, y, z;

	x = block_pos.x;
	y = block_pos.y;
	z = block_pos.z;

	for (int i = 0; i < TREE_HEIGHT; i++)
	{
		if (y + i < CHUNK_SIZE_Y)
		{
			chunk.setBlock({ x, y + i, z }, Block::OAK_LOG);
		}
		else
		{
		
		}
	}
	return true;
}