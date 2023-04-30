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
		.frequency{ 0.006 },
		.octaves{ 4 },
		.lacunarity{ 2.5 },
		.fractal_gain{ 0.7 },
		.weighted_strength{ 0.8 },
	};

}

