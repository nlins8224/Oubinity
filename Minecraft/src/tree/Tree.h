#pragma once
#include <glm/glm.hpp>
#include "../chunk/Chunk.h"

class Tree
{
public:
	Tree(uint8_t height);
	~Tree() = default;
	bool addTree(Chunk& chunk, glm::ivec3 block_pos);
private:
	uint8_t TREE_HEIGHT;
};