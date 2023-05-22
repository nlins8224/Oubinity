#include "NoiseGenerator.h"

NoiseMap NoiseGenerator::generateHeightMap(glm::ivec3 chunk_pos, int chunk_block_amount, NoiseSettings::Settings settings, int seed)
{
	auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
	auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();
	auto fnScale = FastNoise::New<FastNoise::DomainScale>();

	fnFractal->SetSource(fnSimplex);
	fnFractal->SetOctaveCount(settings.octaves);
	fnFractal->SetLacunarity(settings.lacunarity);
	fnFractal->SetGain(settings.fractal_gain);
	fnFractal->SetWeightedStrength(settings.weighted_strength);

	int chunk_block_size = CHUNK_SIZE / chunk_block_amount;
	fnScale->SetSource(fnFractal);
	fnScale->SetScale(chunk_block_size);

	std::vector<float> data_out(CHUNK_SIZE * CHUNK_SIZE);
	glm::ivec3 world_pos = chunk_pos * CHUNK_SIZE;
	fnScale->GenUniformGrid2D(data_out.data(), world_pos.x / chunk_block_size, world_pos.z / chunk_block_size, chunk_block_amount, chunk_block_amount, settings.frequency, seed);
	NoiseMap height_map{};

	for (int x = 0; x < chunk_block_amount; x++)
	{
		for (int z = 0; z < chunk_block_amount; z++)
		{
			height_map[x][z] = data_out[z * chunk_block_amount + x];
		}	
	}

	return height_map;
}