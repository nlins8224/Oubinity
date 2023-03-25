#pragma once
#include "../../third_party/FastNoiseLite.h"

namespace NoiseSettings
{
	struct Settings
	{
		FastNoiseLite::NoiseType noise_type;
		FastNoiseLite::FractalType fractal_type;
		float frequency;
		int octaves;
		float lacunarity;
		float fractal_gain;
		float weighted_strength;
	};

	static const Settings TestSettings
	{
		.noise_type{ FastNoiseLite::NoiseType_Value },
		.fractal_type{ FastNoiseLite::FractalType_FBm },
		.frequency{ 0.006 },
		.octaves{ 4 },
		.lacunarity{ 2.5 },
		.fractal_gain{ 0.7 },
		.weighted_strength{ 0.8 },
	};

	static const Settings BaseSettings
	{
		.noise_type{ FastNoiseLite::NoiseType_Value },
		.fractal_type{ FastNoiseLite::FractalType_FBm },
		.frequency{ 0.002 },
		.octaves{ 4 },
		.lacunarity{ 3.0 },
		.fractal_gain{ 0.5 },
		.weighted_strength{ 0.8 },
	};

	static const Settings FlatSettings
	{
		.noise_type{ FastNoiseLite::NoiseType_OpenSimplex2 },
		.fractal_type{ FastNoiseLite::FractalType_FBm },
		.frequency{ 0.0012 },
		.octaves{ 4 },
		.lacunarity{ 2.5 },
		.fractal_gain{ 0.6 },
		.weighted_strength{ 0.7 },
	};

	static const Settings MountainSettings
	{
		.noise_type{ FastNoiseLite::NoiseType_OpenSimplex2 },
		.fractal_type{ FastNoiseLite::FractalType_FBm },
		.frequency{ 0.004 },
		.octaves{ 1 },
		.lacunarity{ 2.5 },
		.fractal_gain{ 0.2 },
		.weighted_strength{ 0.2 },
	};
}

