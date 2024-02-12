#pragma once
#include <vector>
#include <string>
#include "../../third_party/stb_image.h"
#include "../TerrainGenerationTypes.h"
#include "../../loguru.hpp"
#include "../../chunk/Chunk.h"
#include "../../renderer/ChunkRendererSettings.h"
#include "../../block/Block.h"

namespace PreloadedGeneration
{
	std::vector<BlockMap> parsePNGToBlockMaps(std::string filepath);
	BlockMap parsePNGToBlockMap(unsigned char* chunk_image, int image_width, int image_height, glm::ivec3 chunk_pos_xz, int channels);
	Block::Pixel_RGBA getPixelRGBA(stbi_uc* block_image, int image_width, int image_height, int x, int z, int block_size, int channels);
	block_id pixelToBlock(Block::Pixel_RGBA pixel);
	uint32_t calculateColorDifference(Block::Pixel_RGBA p_one, Block::Pixel_RGBA p_two);
}