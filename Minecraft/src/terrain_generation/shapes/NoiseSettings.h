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

	static const Settings ContinentalnessSettings
	{
		.noise_type{ FastNoiseLite::NoiseType_OpenSimplex2 },
		.fractal_type{ FastNoiseLite::FractalType_FBm },
		.frequency{ 0.004 },
		.octaves{ 3 },
		.lacunarity{ 1.7 },
		.fractal_gain{ 1.0 },
		.weighted_strength{ 0.7 },
	};

	static const Settings ErosionSettings
	{
		.noise_type{ FastNoiseLite::NoiseType_Value },
		.fractal_type{ FastNoiseLite::FractalType_FBm },
		.frequency{ 0.004 },
		.octaves{ 2 },
		.lacunarity{ 3.5 },
		.fractal_gain{ 1.5 },
		.weighted_strength{ 1.0 },
	};

	static const Settings TestSettings
	{
		.noise_type{ FastNoiseLite::NoiseType_Value },
		.fractal_type{ FastNoiseLite::FractalType_FBm },
		.frequency{ 0.008 },
		.octaves{ 4 },
		.lacunarity{ 0.2 },
		.fractal_gain{ 0.9 },
		.weighted_strength{ 0.0 },
	};
}

