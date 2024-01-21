#pragma once
#include <vector>
#include <string>
#include "../../third_party/stb_image.h"
#include "../TerrainGenerationTypes.h"
#include "../../loguru.hpp"

namespace PreloadedGeneration 
{
	std::vector<HeightMap> parsePNGToHeightMaps(std::string filepath);
	HeightMap parsePNGToHeightMap(unsigned char* chunk_height_map, int width);
}