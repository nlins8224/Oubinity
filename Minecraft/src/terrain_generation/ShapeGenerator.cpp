#include "ShapeGenerator.h"

ShapeGenerator::ShapeGenerator(int seed)
	: m_seed{seed},
	m_spline{}
{
	// TODO: Move this up to TerrainGenerator, this can be calculated only once.
	m_continentalness_range = m_spline.getBezierRange(Nodes::continentalness);
	m_erosion_range = m_spline.getBezierRange(Nodes::erosion);
}

HeightMap ShapeGenerator::generateSurfaceMap(Chunk& chunk)
{
	NoiseGenerator noise_generator;

	HeightMap continentalness_map = noise_generator.generateHeightMap(chunk.getWorldPos(), NoiseSettings::ContinentalnessSettings, m_seed);
	HeightMap surface_map{};

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int z = 0; z < CHUNK_SIZE_Z; z++)
		{
			int height_key_c = continentalness_map[x][z] * m_spline.getKeyTranslateValue();
			double continentalness_height = m_continentalness_range[height_key_c];
			surface_map[x][z] = continentalness_height;
		}
	}
	
	return surface_map;
}
