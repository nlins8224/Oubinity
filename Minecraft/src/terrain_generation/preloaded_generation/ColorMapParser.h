#pragma once
#include <vector>
#include <string>
#include "../TerrainGenerationTypes.h"
#include "../../third_party/stb_image.h"
#include "glm/glm.hpp"

namespace PreloadedGeneration
{
	std::vector<BlockMap> parsePNGToBlockMaps(std::string filepath, glm::vec3 scale);
	BlockMap parsePNGToBlockMap(unsigned char* chunk_image, int image_width, int image_height, glm::ivec3 chunk_pos_xz, int channels, glm::vec3 scale);
	Block::Pixel_RGBA getPixelRGBA(stbi_uc* block_image, int image_width, int image_height, int x, int z, int block_size, int channels);
	block_id pixelToBlock(Block::Pixel_RGBA pixel);
	uint32_t calculateColorDifference(Block::Pixel_RGBA p_one, Block::Pixel_RGBA p_two);
}