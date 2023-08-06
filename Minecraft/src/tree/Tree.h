//#pragma once
//#include <glm/glm.hpp>
//#include "../chunk/Chunk.h"
//#include "../chunk/ChunksMap.h" // Include this for glm::ivec3 hash, TODO: Refactor
//#include <unordered_set>
//
//class Tree
//{
//public:
//	Tree(uint8_t trunk_height, uint8_t crown_height, uint8_t crown_width);
//	~Tree() = default;
//	bool addTree(Chunk& chunk, glm::ivec3 block_pos);
//private:
//	std::unordered_set<glm::ivec3> addTrunk(Chunk& chunk, glm::ivec3 block_pos);
//	std::unordered_set<glm::ivec3> addCrown(Chunk& chunk, glm::ivec3 block_pos);
//	void placeBlock(Chunk& chunk, glm::ivec3 block_pos, Block::block_id block_type, std::unordered_set<glm::ivec3>& chunks_to_update);
//	int determineChunkOffset(int block_pos);
//	int determineBlockOffset(int block_pos);
//	bool shouldCutBlock(int x, int y, int z);
//	uint8_t TRUNK_HEIGHT;
//	uint8_t CROWN_HEIGHT;
//	uint8_t CROWN_WIDTH;
//};