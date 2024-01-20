#include "ShapeGenerator.h"

ShapeGenerator::ShapeGenerator(int seed)
	: m_seed{seed}
{
}

HeightMap ShapeGenerator::generateSurfaceMap(Chunk& chunk)
{
	glm::ivec2 chunk_pos_xz = chunk.getPosXZ();

	HeightMap base_map = generateHeightMap(chunk.getPos(), chunk.getLevelOfDetail(), NoiseSettings::TestSettings, m_seed);
	HeightMap surface_map{};

	// Temporary
	int chunk_block_amount = chunk.getLevelOfDetail().block_amount;
	for (int x = 0; x < chunk_block_amount; x++)
	{
		for (int z = 0; z < chunk_block_amount; z++)
		{
			float base_height = ((base_map[x][z] + 1.0f) / 2) * 300.0f;
			surface_map[x][z] = base_height;
		}
	}

	return surface_map;
}

HeightMap ShapeGenerator::generateHeightMap(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod, NoiseSettings::Settings settings, int seed)
{
	auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
	auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();
	auto fnScale = FastNoise::New<FastNoise::DomainScale>();

	fnFractal->SetSource(fnSimplex);
	fnFractal->SetOctaveCount(settings.octaves);
	fnFractal->SetLacunarity(settings.lacunarity);
	fnFractal->SetGain(settings.fractal_gain);
	fnFractal->SetWeightedStrength(settings.weighted_strength);

	fnScale->SetSource(fnFractal);
	fnScale->SetScale(lod.block_size);

	std::vector<float> data_out(CHUNK_SIZE * CHUNK_SIZE);
	glm::ivec3 world_pos = chunk_pos * CHUNK_SIZE;
	fnScale->GenUniformGrid2D(data_out.data(), chunk_pos.x * lod.block_amount, chunk_pos.z * lod.block_amount, lod.block_amount, lod.block_amount, settings.frequency , seed);
	HeightMap height_map{};

	for (int x = 0; x < lod.block_amount; x++)
	{
		for (int z = 0; z < lod.block_amount; z++)
		{
			height_map[x][z] = data_out[z * lod.block_amount + x];
		}
	}

	return height_map;
}