#include "WorldGenerator.h"

height_map WorldGenerator::generateChunkHeightMap(glm::ivec3 chunk_pos, const int world_seed) const
{
	FastNoiseLite noise(world_seed);
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetFrequency(0.005);
	noise.SetFractalOctaves(10);
	noise.SetFractalLacunarity(2.0);

	height_map h_map{};
	double target_height = 0.0;

	for (int x = 0; x < Chunk::CHUNK_SIZE_X; x++)
	{
		for (int z = 0; z < Chunk::CHUNK_SIZE_Z; z++)
		{
			float x_world_pos = x + chunk_pos.x * Chunk::CHUNK_SIZE_X;
			float z_world_pos = z + chunk_pos.z * Chunk::CHUNK_SIZE_Z;

			target_height = 20 * noise.GetNoise(x_world_pos, z_world_pos) + 100;
			h_map[x][z] = target_height;
		}
	}

	return h_map;
}
