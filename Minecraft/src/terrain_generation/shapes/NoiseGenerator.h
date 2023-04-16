#pragma once
#include <FastNoise/FastNoise.h>
#include "glm/glm.hpp"
#include "NoiseSettings.h"
#include "../NoiseMapTypes.h"
class NoiseGenerator
{
public:
	HeightMap generateHeightMap(glm::ivec3 chunk_pos, int seed, float frequency = 0.002);

private:
	float m_frequency;
};