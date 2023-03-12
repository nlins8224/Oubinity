#include "NoiseGenerator.h"

DensityMap NoiseGenerator::generateDensityMap(glm::ivec3 chunk_pos, int seed)
{
	FastNoiseLite noise(seed);
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetFrequency(0.005);
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	noise.SetFractalOctaves(3);
	noise.SetFractalLacunarity(2.5);
	noise.SetFractalGain(0.5f);

	DensityMap density_map{};

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = 0; z < CHUNK_SIZE_Z; z++)
			{
				float x_world_pos = x + chunk_pos.x;
				float y_world_pos = y + chunk_pos.y;
				float z_world_pos = z + chunk_pos.z;

				density_map[x][y][z] = noise.GetNoise(x_world_pos, y_world_pos, z_world_pos);
			}
		}
	}

	return density_map;
}

HeightMap NoiseGenerator::generateHeightMap(glm::ivec3 chunk_pos, NoiseSettings::Settings noise_settings, int seed)
{
	FastNoiseLite noise(seed);
	noise.SetNoiseType(noise_settings.noise_type);
	noise.SetFractalType(noise_settings.fractal_type);
	noise.SetFrequency(noise_settings.frequency);
	noise.SetFractalOctaves(noise_settings.octaves);
	noise.SetFractalLacunarity(noise_settings.lacunarity);
	noise.SetFractalGain(noise_settings.fractal_gain);
	noise.SetFractalWeightedStrength(noise_settings.weighted_strength);

	HeightMap height_map{};

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int z = 0; z < CHUNK_SIZE_Z; z++)
		{
			float x_world_pos = x + chunk_pos.x;
			float z_world_pos = z + chunk_pos.z;

			height_map[x][z] = noise.GetNoise(x_world_pos, z_world_pos);
		}	
	}

	return height_map;
}


