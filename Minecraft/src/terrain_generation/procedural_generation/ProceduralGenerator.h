#pragma once
#include "../../chunk/Chunk.h"
#include "../../chunk/ChunkSize.h"
#include "ShapeGenerator.h"
#include "LayerGenerator.h"
#include "DecorationGenerator.h"

class ProceduralGenerator
{
public:
	ProceduralGenerator();
	ProceduralGenerator(int world_seed, uint8_t water_height);
	virtual ~ProceduralGenerator() = default;
	HeightMap generateHeightMap(Chunk& chunk);
	void generateLayers(Chunk& chunk, HeightMap height_map);
	void generateLayers(Chunk& chunk, HeightMap height_map, BlockMap block_map);
	void generateTrees(Chunk& chunk);
	void generateTrees(Chunk& chunk, HeightMap& height_map);
	uint8_t getWaterHeight();
private:
	int m_world_seed;
	uint8_t m_water_height;
	ShapeGenerator m_shape_generator;
	DecorationGenerator m_decoration_generator;
	std::unordered_map<glm::ivec3, HeightMap> m_heightmaps;
};

