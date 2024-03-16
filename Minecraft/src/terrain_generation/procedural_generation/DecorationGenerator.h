#pragma once
#include "ShapeGenerator.h"
#include "../TerrainGenerationTypes.h"
#include "../../chunk/Chunk.h"
#include "../../tree/Tree.h"

struct TreeShape
{
	uint8_t trunk_height;
	uint8_t crown_height;
	uint8_t crown_width;
};

struct TreeHeightBoundaries
{
	int height_min;
	int height_max;
};

class DecorationGenerator
{
public:
	DecorationGenerator();
	virtual ~DecorationGenerator() = default;
	void generateTrees(Chunk& chunk, HeightMap& surface_map, TreePresenceMap tree_presence_map, uint8_t water_height);
private:
	TreeShape m_tree_shape;
	TreeHeightBoundaries m_tree_height_boundaries;

};