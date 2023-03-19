#include "MaskEffect.h"

void MaskEffect::addEffect(HeightMap& height_map)
{
	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int z = 0; z < CHUNK_SIZE_Z; z++)
		{
			height_map[x][z] = height_map[x][z] > 0.2f ? 1.0f : 0.0f;
		}
	}
}