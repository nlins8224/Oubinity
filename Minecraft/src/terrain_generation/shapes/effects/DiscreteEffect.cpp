#include "DiscreteEffect.h"

void DiscreteEffect::addEffect(HeightMap& height_map)
{
	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int z = 0; z < CHUNK_SIZE_Z; z++)
		{
			height_map[x][z] = truncate(height_map[x][z]);
		}
	}
}

double DiscreteEffect::truncate(double value)
{
	return floorf(value * 1000) / 1000;
}
