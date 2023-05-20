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

	std::vector<float> data_out(CHUNK_SIZE * CHUNK_SIZE);
	fnFractal->GenUniformGrid2D(data_out.data(), chunk_pos.x * CHUNK_SIZE, chunk_pos.z * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, settings.frequency, seed);
	NoiseMap height_map{};

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			height_map[x][z] = data_out[z * CHUNK_SIZE + x];
		}	
	}

	return height_map;
}