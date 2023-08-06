//#pragma once
//#include <FastNoise/FastNoise.h>
//#include <glm/glm.hpp>
//#include "../../chunk/Chunk.h"
//#include "../NoiseMapTypes.h"
//#include "NoiseGenerator.h"
//#include "../../tree/Tree.h"
//
//struct TreeShape
//{
//	uint8_t trunk_height;
//	uint8_t crown_height;
//	uint8_t crown_width;
//};
//
//struct TreeHeightBoundaries
//{
//	int height_min;
//	int height_max;
//};
//
//class DecorationGenerator
//{
//public:
//	DecorationGenerator(TreeShape tree_shape, TreeHeightBoundaries tree_height_boundaries, int seed);
//	~DecorationGenerator() = default;
//	void decorateChunkTerrain(Chunk& chunk, NoiseMap& surface_map);
//private:
//	int m_seed;
//	TreeShape m_tree_shape;
//	TreeHeightBoundaries m_tree_height_boundaries;
//	bool isOnSurfaceChunk(int chunk_pos_y, int height);
//};