#pragma once
#include "../chunk/Chunk.h"
#include "../chunk/ChunkSize.h"
#include "../third_party/FastNoiseLite.h"
#include "shapes/NoiseGenerator.h"
#include "shapes/Spline.h"
#include "BiomeGenerator.h";
#include "ShapeGenerator.h"


class TerrainGenerator
{
public:
	TerrainGenerator(int world_seed, const int render_distance);
	~TerrainGenerator() = default;
	void generateChunkTerrain(Chunk& chunk, const int render_distance);
	ShapeGenerator& getShapeGenerator();
	float getSurfaceHeight(glm::ivec2 chunk_pos_xz, glm::ivec2 block_pos_xz);
private:
	int m_world_seed;
	ShapeGenerator m_shape_generator;
};

