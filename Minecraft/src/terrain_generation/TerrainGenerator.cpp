#include "TerrainGenerator.h"

height_map TerrainGenerator::generateChunkHeightMap(glm::ivec3 chunk_pos, const int world_seed) const
{
	FastNoiseLite noise(world_seed);
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetFrequency(0.005);
	noise.SetFractalOctaves(6);
	noise.SetFractalLacunarity(2.0);
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);

	height_map h_map{};
	double target_height = 0.0;

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int z = 0; z < CHUNK_SIZE_Z; z++)
		{
			float x_world_pos = x + chunk_pos.x;
			float z_world_pos = z + chunk_pos.z;

			target_height = 10 * noise.GetNoise(x_world_pos, z_world_pos) + 35;
			h_map[x][z] = target_height;
			//std::cout << target_height << std::endl;
		}
	}

	return h_map;
}

TerrainGenerator::TerrainGenerator(int world_seed, const int render_distance)
	: m_world_seed{world_seed}
{
}

void TerrainGenerator::generateChunkTerrain(Chunk& chunk, const int render_distance)
{

	if (chunk.isTerrainGenerated())
		return;

	height_map height_map{ generateChunkHeightMap(chunk.getWorldPos(), m_world_seed) };
	BiomeGenerator biome_generator;
	biome_generator.processChunk(chunk, height_map);

	chunk.setIsTerrainGenerated(true);
}

