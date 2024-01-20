#pragma once
#include <vector>
#include <string>
#include "../../third_party/stb_image.h"
#include "../TerrainGenerationTypes.h"
#include "../../loguru.hpp"

namespace HeightMapParser 
{
	std::vector<HeightMap> parsePNGToHeightMaps(std::string filepath);
	HeightMap parsePNGToHeightMap(unsigned char* png_image, int height_offset, int width_offset);
}