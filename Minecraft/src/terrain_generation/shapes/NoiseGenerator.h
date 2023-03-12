#pragma once
#include "glm/glm.hpp"
#include "NoiseSettings.h"
#include "../NoiseMapTypes.h"
#include "../../third_party/FastNoiseLite.h"
#include <iostream>

class NoiseGenerator
{
public:
	DensityMap generateDensityMap(glm::ivec3 chunk_pos, int seed);
	HeightMap generateHeightMap(glm::ivec3 chunk_pos, NoiseSettings::Settings noise_settings, int seed);

private:

};