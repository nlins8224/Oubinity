#include "NoiseGenerator.h"

DensityMap NoiseGenerator::generateDensityMap(glm::ivec3 chunk_pos, NoiseSettings::Settings noise_settings, int seed)
{
	FastNoiseLite noise(seed);
	setSettings(noise, noise_settings);

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
	setSettings(noise, noise_settings);

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

void NoiseGenerator::setSettings(FastNoiseLite& noise, NoiseSettings::Settings settings)
{
	noise.SetNoiseType(settings.noise_type);
	noise.SetFractalType(settings.fractal_type);
	noise.SetFrequency(settings.frequency);
	noise.SetFractalOctaves(settings.octaves);
	noise.SetFractalLacunarity(settings.lacunarity);
	noise.SetFractalGain(settings.fractal_gain);
	noise.SetFractalWeightedStrength(settings.weighted_strength);
}


