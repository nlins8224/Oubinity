#include "ShapeGenerator.h"
#include <iostream>

ShapeGenerator::ShapeGenerator(int seed)
	: m_seed{seed},
	m_spline{},
	m_surface_maps{}
{
	m_base_range = m_spline.getLinearRange(SplineControlPoints::test);
	m_flat_range = m_spline.getLinearRange(SplineControlPoints::flat);
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
	m_basic_layer_maps[chunk_pos_xz] = base_map;
}

float ShapeGenerator::getSurfaceHeight(glm::ivec2 chunk_pos_xz, glm::ivec2 block_pos_xz)
{
	if (m_surface_maps.find(chunk_pos_xz) == m_surface_maps.end())
	{
		std::cout << "Surface map at x: " << chunk_pos_xz.x << " z: " << chunk_pos_xz.y << " not found!" << std::endl;
		return -1;
	}
		

	int x = block_pos_xz[0];
	int z = block_pos_xz[1];
	return m_surface_maps[chunk_pos_xz][x][z];
}

float ShapeGenerator::getBasicNoiseValue(glm::ivec2 chunk_pos_xz, glm::ivec2 block_pos_xz)
{
	if (m_basic_layer_maps.find(chunk_pos_xz) == m_basic_layer_maps.end())
		return -1;

	int x = block_pos_xz[0];
	int z = block_pos_xz[1];
	return m_basic_layer_maps[chunk_pos_xz][x][z];
}


NoiseMap& ShapeGenerator::getSurfaceMap(glm::ivec2 chunk_pos_xz)
{
	return m_surface_maps.at(chunk_pos_xz);
}

ChunkHeightMaps& ShapeGenerator::getSurfaceMaps()
{
	return m_surface_maps;
}

int ShapeGenerator::getSeed()
{
	return m_seed;
}
