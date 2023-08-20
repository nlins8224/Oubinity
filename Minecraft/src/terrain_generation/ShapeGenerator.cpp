#include "ShapeGenerator.h"
#include <iostream>

ShapeGenerator::ShapeGenerator(int seed)
	: m_seed{seed},
	m_surface_maps{}
{
}

void ShapeGenerator::generateSurfaceMap(Chunk& chunk)
{
	glm::ivec2 chunk_pos_xz = chunk.getPosXZ();
	if (m_surface_maps.find(chunk_pos_xz) != m_surface_maps.end())
		return;

	NoiseGenerator noise_generator;
	int chunk_block_amount = chunk.getLevelOfDetail().block_amount;
	NoiseMap base_map = noise_generator.generateHeightMap(chunk.getPos(), chunk_block_amount, NoiseSettings::TestSettings, m_seed);
	NoiseMap surface_map{};

	// Temporary
	for (int x = 0; x < chunk_block_amount; x++)
	{
		for (int z = 0; z < chunk_block_amount; z++)
		{
			float base_height = ((base_map[x][z] + 1.0f) / 2) * 100.0f;
			surface_map[x][z] = base_height;
		}
	}

	m_surface_maps[chunk_pos_xz] = surface_map;
}

NoiseMap& ShapeGenerator::getSurfaceMap(glm::ivec2 chunk_pos_xz)
{
	return m_surface_maps.at(chunk_pos_xz);
}