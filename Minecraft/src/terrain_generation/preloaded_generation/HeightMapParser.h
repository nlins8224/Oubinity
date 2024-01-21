#pragma once
#include <vector>
#include <string>
#include "../../third_party/stb_image.h"
#include "../TerrainGenerationTypes.h"
#include "../../loguru.hpp"
#include "../../chunk/Chunk.h"
#include "../../renderer/ChunkRendererSettings.h"

namespace PreloadedGeneration 
{
	std::vector<HeightMap> parsePNGToHeightMaps(std::string filepath);
	HeightMap parsePNGToHeightMap(unsigned char* chunk_image, int width, glm::ivec3 chunk_pos_xz);
}