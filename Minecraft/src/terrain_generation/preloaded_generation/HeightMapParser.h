#pragma once
#include <vector>
#include <string>
#include "../TerrainGenerationTypes.h"
#include "glm/glm.hpp"


namespace PreloadedGeneration 
{
	std::vector<HeightMap> parsePNGToHeightMaps_8BIT(std::string filepath, glm::vec3 scale);
	HeightMap parsePNGToHeightMap_8BIT(unsigned char* chunk_image, int width, glm::ivec3 chunk_pos_xz, glm::vec3 scale);
}