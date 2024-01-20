#pragma once
#include "../../chunk/Chunk.h"
#include "../../chunk/ChunkSize.h"
#include "ShapeGenerator.h"
#include "LayerGenerator.h"

class ProceduralGenerator
{
public:
	ProceduralGenerator();
	ProceduralGenerator(int world_seed, uint8_t surface_height, uint8_t water_height);
	virtual ~ProceduralGenerator() = default;
	NoiseMap generateHeightMap(Chunk& chunk);
	void generateLayers(Chunk& chunk, NoiseMap height_map);
private:
	int m_world_seed;
	uint8_t m_min_surface_height;
	uint8_t m_water_height;
	ShapeGenerator m_shape_generator;
};

