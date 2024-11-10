#include "ProceduralGenerator.h"

ProceduralGenerator::ProceduralGenerator(int world_seed, uint8_t water_height)
	: m_world_seed{ world_seed },
	m_water_height{ water_height }
{
}

ProceduralGenerator::ProceduralGenerator()
	: m_world_seed{ 1337 },
	m_water_height{ 10 }
{

}

HeightMap ProceduralGenerator::generateHeightMap(Chunk& chunk)
{
	glm::ivec2 chunk_pos_xz = chunk.getPosXZ();

	HeightMap base_map = generateHeightMap(chunk.getPos(), chunk.getLevelOfDetail(), NoiseSettings::TestSettings, m_world_seed);
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

HeightMap ProceduralGenerator::generateHeightMap(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod)
{
	glm::ivec2 chunk_pos_xz = { chunk_pos.x, chunk_pos.z };

	HeightMap base_map = generateHeightMap(chunk_pos, lod, NoiseSettings::TestSettings, m_world_seed);
	HeightMap surface_map{};

	// Temporary
	int chunk_block_amount = lod.block_amount;
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

HeightMap ProceduralGenerator::generateHeightMap(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod, NoiseSettings::Settings settings, int seed)
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
	fnScale->GenUniformGrid2D(data_out.data(), chunk_pos.x * lod.block_amount, chunk_pos.z * lod.block_amount, lod.block_amount, lod.block_amount, settings.frequency, seed);
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

bool ProceduralGenerator::generateLayers(Chunk& chunk, HeightMap height_map)
{
	int block_amount = chunk.getLevelOfDetail().block_amount;
	int block_size = chunk.getLevelOfDetail().block_size;
	glm::ivec3 chunk_world_pos = chunk.getPos() * CHUNK_SIZE;

	bool anything_added = false;
	for (int x = 0; x < block_amount; x++)
	{
		for (int y = 0; y < block_amount; y++)
		{
			for (int z = 0; z < block_amount; z++)
			{
				glm::ivec3 block_pos = { x, y, z };
				int surface_height = height_map[x][z];
				glm::ivec3 block_world_pos = chunk_world_pos + (block_pos * block_size);
				if (surface_height > block_world_pos.y - block_size && surface_height < block_world_pos.y + block_size)
				{
					chunk.setBlock(block_pos, Block::GRASS);
					anything_added = true;
				}
				else {
					chunk.setBlock(block_pos, Block::AIR);
				}
			}
		}
	}
	return anything_added;
}

TreePresenceMap ProceduralGenerator::generateTreePresenceMap(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod, NoiseSettings::Settings settings, int seed) {
	HeightMap height_map = generateHeightMap(chunk_pos, lod, settings, seed);
	TreePresenceMap tree_presence_map{};
	for (int x = 0; x < lod.block_amount; x++)
	{
		for (int z = 0; z < lod.block_amount; z++)
		{
			float height_normalized = (height_map[x][z] / 2.0) + 0.5;
			tree_presence_map[x][z] = (int)(height_normalized * 1000) % (250 / (lod.divide_factor * 2)) == 0;
		}
	}
	return tree_presence_map;
}

void ProceduralGenerator::generateTrees(Chunk& chunk)
{
	glm::ivec3 chunk_pos = chunk.getPos();
	LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
	TreePresenceMap tree_presence_map = generateTreePresenceMap(chunk_pos, lod, NoiseSettings::TreeSettings, m_world_seed);
	HeightMap height_map = generateHeightMap(chunk);

	m_decoration_generator.generateTrees(chunk, height_map, tree_presence_map, m_water_height);
}

void ProceduralGenerator::generateTrees(Chunk& chunk, HeightMap& height_map)
{
	glm::ivec3 chunk_pos = chunk.getPos();
	LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
	TreePresenceMap tree_presence_map = generateTreePresenceMap(chunk_pos, lod, NoiseSettings::TreeSettings, m_world_seed);
	m_decoration_generator.generateTrees(chunk, height_map, tree_presence_map, m_water_height);
}

uint8_t ProceduralGenerator::getWaterHeight()
{
	return m_water_height;
}