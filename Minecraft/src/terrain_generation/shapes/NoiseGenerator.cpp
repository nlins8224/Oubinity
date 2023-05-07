#include "NoiseGenerator.h"

NoiseMap NoiseGenerator::generateHeightMap(glm::ivec3 chunk_pos, NoiseSettings::Settings settings, int seed)
{
	auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
	auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();

	fnFractal->SetSource(fnSimplex);
	fnFractal->SetOctaveCount(settings.octaves);
	fnFractal->SetLacunarity(settings.lacunarity);
	fnFractal->SetGain(settings.fractal_gain);
	fnFractal->SetWeightedStrength(settings.weighted_strength);

	std::vector<float> data_out(CHUNK_SIZE_X * CHUNK_SIZE_Z);
	fnFractal->GenUniformGrid2D(data_out.data(), chunk_pos.x * CHUNK_SIZE_X, chunk_pos.z * CHUNK_SIZE_Z, CHUNK_SIZE_X, CHUNK_SIZE_Z, settings.frequency, seed);
	NoiseMap height_map{};

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int z = 0; z < CHUNK_SIZE_Z; z++)
		{
			height_map[x][z] = data_out[z * CHUNK_SIZE_X + x];
		}	
	}

	return height_map;
}