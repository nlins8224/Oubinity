#pragma once
#include <FastNoise/FastNoise.h>
#include "../block/Block.h"
#include "../chunk/Chunk.h"
#include "../level_of_detail/LevelOfDetail.h"

namespace std
{
	template<>
	struct hash<glm::ivec2>
	{
		const size_t operator()(const glm::ivec2& vec) const
		{
			size_t res = 17;
			res = res * 31 + std::hash<int>()(vec.x);
			res = res * 31 + std::hash<int>()(vec.y);
			return res;
		};
	};
}

using NoiseMap = std::array<std::array<double, CHUNK_SIZE>, CHUNK_SIZE>;
using ChunkHeightMaps = std::unordered_map<glm::ivec2, NoiseMap>;

namespace NoiseSettings
{
	struct Settings
	{
		float frequency;
		int octaves;
		float lacunarity;
		float fractal_gain;
		float weighted_strength;
	};

	static const Settings TestSettings
	{
		.frequency{ 0.0005 },
		.octaves{ 4 },
		.lacunarity{ 2.1 },
		.fractal_gain{ 0.8 },
		.weighted_strength{ 0.1 },
	};

	static const Settings TreeSettings
	{
		.frequency{ 0.02 },
		.octaves{ 3 },
		.lacunarity{ 2.0 },
		.fractal_gain{ 0.5 },
		.weighted_strength{ 0.0 },
	};

}


class ShapeGenerator
{
public:
	ShapeGenerator(int seed);
	~ShapeGenerator() = default;
	NoiseMap generateSurfaceMap(Chunk& chunk);
	NoiseMap generateHeightMap(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod, NoiseSettings::Settings settings, int seed);

private:
	int m_seed;
};