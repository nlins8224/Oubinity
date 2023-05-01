#pragma once
#include "../chunk/Chunk.h"
#include "../chunk/ChunkSize.h"
#include "shapes/NoiseGenerator.h"
#include "shapes/Spline.h"
#include "BiomeGenerator.h";
#include "ShapeGenerator.h"
#include "../tree/Tree.h"


class TerrainGenerator
{
public:
	TerrainGenerator(int world_seed, uint8_t surface_height, uint8_t water_height);
	~TerrainGenerator() = default;
	void generateChunkTerrain(Chunk& chunk);
	void decorateChunkTerrain(Chunk& chunk);
	ShapeGenerator& getShapeGenerator();
	float getSurfaceHeight(glm::ivec2 chunk_pos_xz, glm::ivec2 block_pos_xz);
	bool isOnSurfaceChunk(int chunk_pos_y, int height);
private:
	int m_world_seed;
	uint8_t m_min_surface_height;
	uint8_t m_water_height;
	ShapeGenerator m_shape_generator;
};

