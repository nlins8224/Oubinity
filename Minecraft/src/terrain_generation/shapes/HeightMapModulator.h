#pragma once
#include <functional>
#include "../NoiseMapTypes.h"
#include "../../chunk/ChunkSize.h"

namespace HeightMapModulator
{
	void applyModulator(HeightMap& height_map, std::function<double(double)> modulator)
	{
		for (int x = 0; x < CHUNK_SIZE_X; x++)
		{
			for (int z = 0; z < CHUNK_SIZE_Z; z++)
			{
				height_map[x][z] = modulator(height_map[x][z]);
			}
		}
	}

	double easeInOutQuad(double x)
	{
		return x < 0.5 ? 
			2 * x * x : 
			1 - pow(-2 * x + 2, 2) / 2;
	}
}