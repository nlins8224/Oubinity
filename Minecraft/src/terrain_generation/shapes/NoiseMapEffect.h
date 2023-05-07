#pragma once
#include <functional>
#include "../NoiseMapTypes.h"
#include "../../chunk/ChunkSize.h"

namespace NoiseMapEffect
{
	NoiseMap applyEffect(std::function<double(double)> effect)
	{
		NoiseMap noise_map = {};
		for (int x = 0; x < CHUNK_SIZE_X; x++)
		{
			for (int z = 0; z < CHUNK_SIZE_Z; z++)
			{
				noise_map[x][z] = effect(noise_map[x][z]);
			}
		}

		return noise_map;
	}

	double easeInOutQuad(double x)
	{
		return x < 0.5 ? 
			2 * x * x : 
			1 - pow(-2 * x + 2, 2) / 2;
	}
}