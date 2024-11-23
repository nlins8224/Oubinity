#pragma once
#include <glm/glm.hpp>
#include "../chunk/Chunk.h"
#include "../chunk/ChunksMap.h" // Include this for glm::ivec3 hash, TODO: Refactor
#include <unordered_set>
#include "BranchGenerator.h"

class Tree
{
public:
	Tree(uint8_t crown_height, uint8_t crown_width);
	virtual ~Tree() = default;
	bool addTree(Chunk& chunk, glm::ivec3 block_pos);
private:
	void addCrown(Chunk& chunk, glm::ivec3 block_pos);
	void addCrowns(Chunk& chunk, std::vector<ProceduralTree::Branch> branches);
	void voxelizeBranches(Chunk& chunk, std::vector<ProceduralTree::Branch> branches);
	void voxelizeBranch(Chunk& chunk, ProceduralTree::Branch branch); 
	void placeBlock(Chunk& chunk, glm::ivec3 block_pos, Block::block_id block_type);
	int determineChunkOffset(int block_pos);
	int determineBlockOffset(int block_pos);
	bool shouldCutBlock(int x, int y, int z);
	uint8_t CROWN_HEIGHT;
	uint8_t CROWN_WIDTH;

	ProceduralTree::BranchGenerator m_branch_generator;
};