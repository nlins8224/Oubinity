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
	std::vector<HeightMap> parsePNGToHeightMaps_8BIT(std::string filepath, glm::vec3 scale);
	std::vector<HeightMap> parsePNGToHeightMaps_16BIT(std::string filepath, double y_scale);
	HeightMap parsePNGToHeightMap_8BIT(unsigned char* chunk_image, int width, glm::ivec3 chunk_pos_xz, glm::vec3 scale);
	HeightMap parsePNGToHeightMap_16BIT(uint16_t* chunk_image, int width, glm::ivec3 chunk_pos_xz, double y_scale);

}