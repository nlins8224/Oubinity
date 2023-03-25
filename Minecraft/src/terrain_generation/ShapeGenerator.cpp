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
	NoiseGenerator noise_generator;

	HeightMap base_map = noise_generator.generateHeightMap(chunk.getWorldPos(), NoiseSettings::TestSettings, m_seed);
	HeightMap flat_map = noise_generator.generateHeightMap(chunk.getWorldPos(), NoiseSettings::FlatSettings, m_seed);
	HeightMap mountain_map = noise_generator.generateHeightMap(chunk.getWorldPos(), NoiseSettings::MountainSettings, m_seed);

	HeightMap surface_map{};

	// Temporary
	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int z = 0; z < CHUNK_SIZE_Z; z++)
		{
			//if (base_map[x][z] < 0.47f)
			//{
			//	float lerp = std::lerp((float)base_map[x][z], (float)flat_map[x][z], 0.2f);
			//	base_map[x][z] = lerp > 0.5f ? 0.5f : lerp;
			//}
		
			int height_key_base = base_map[x][z] * m_spline.getKeyTranslateValue();
			int height_key_flat = flat_map[x][z] * m_spline.getKeyTranslateValue();

			float base_height = m_base_range[height_key_base];
			float flat_height = m_flat_range[height_key_flat];

			surface_map[x][z] = base_height;
		}
	}
	
	glm::ivec2 chunk_pos_xz = chunk.getPosXZ();
	m_surface_maps[chunk_pos_xz] = surface_map;
	m_basic_layer_maps[chunk_pos_xz] = base_map;
}

float ShapeGenerator::getSurfaceHeight(glm::ivec2 chunk_pos_xz, glm::ivec2 block_pos_xz)
{
	if (m_surface_maps.find(chunk_pos_xz) == m_surface_maps.end())
		return -1;

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


HeightMap& ShapeGenerator::getSurfaceMap(glm::ivec2 chunk_pos_xz)
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
