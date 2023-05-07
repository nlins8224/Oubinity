#pragma once
#include <FastNoise/FastNoise.h>

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
		.frequency{ 0.002 },
		.octaves{ 5 },
		.lacunarity{ 2.0 },
		.fractal_gain{ 0.5 },
		.weighted_strength{ 0.0 },
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

