#pragma once
#include "NoiseMapTypes.h"
#include "shapes/NoiseGenerator.h"
#include "shapes/Nodes.h"
#include "../block/Block.h"
#include "../chunk/Chunk.h"

class ShapeGenerator
{
public:
	ShapeGenerator(int seed);
	~ShapeGenerator() = default;
	HeightMap generateSurfaceMap(Chunk& chunk);
private:
	int m_seed;
	Spline m_spline;
	SplineRange m_continentalness_range;
	SplineRange m_erosion_range;

};