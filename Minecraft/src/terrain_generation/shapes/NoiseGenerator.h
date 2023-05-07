#pragma once
#include <FastNoise/FastNoise.h>
#include "glm/glm.hpp"
#include "NoiseSettings.h"
#include "../NoiseMapTypes.h"
class NoiseGenerator
{
public:
	NoiseMap generateHeightMap(glm::ivec3 chunk_pos, NoiseSettings::Settings settings, int seed);
private:
};