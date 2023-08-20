#pragma once
#include "../chunk/Chunk.h"
#include "../chunk/ChunkSize.h"
#include "BiomeGenerator.h";
#include "ShapeGenerator.h";

class TerrainGenerator
{
public:
	TerrainGenerator();
	TerrainGenerator(int world_seed, uint8_t surface_height, uint8_t water_height);
	~TerrainGenerator() = default;
	void generateChunkTerrain(Chunk& chunk);
	ShapeGenerator& getShapeGenerator();
private:
	int m_world_seed;
	uint8_t m_min_surface_height;
	uint8_t m_water_height;
	ShapeGenerator m_shape_generator;
};

