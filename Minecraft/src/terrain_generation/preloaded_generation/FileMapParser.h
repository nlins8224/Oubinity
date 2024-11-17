#pragma once
#include <vector>
#include <string>
#include "../TerrainGenerationTypes.h"
#include "../../third_party/stb_image.h"
#include "glm/glm.hpp"

namespace PreloadedGeneration
{
	enum class image_type {
		GRAYSCALE,
		COLOR
	};

	struct ImageBundle {
		int width;
		int height;
		int channels;
		unsigned char* image;
	};

	// Grayscale
	HeightMapBundle parsePNGToHeightMaps_8BIT(std::string filepath, glm::vec3 scale);
	HeightMap parsePNGToHeightMap_8BIT(unsigned char* chunk_image, int width, glm::ivec3 chunk_pos_xz, glm::vec3 scale);

	// Color
	BlockMapBundle parsePNGToBlockMaps(std::string filepath, glm::vec3 scale);
	BlockMap parsePNGToBlockMap(unsigned char* chunk_image, int image_width, int image_height, glm::ivec3 chunk_pos_xz, int channels);
	Block::Pixel_RGBA getPixelRGBA(stbi_uc* block_image, int image_width, int image_height, int x, int z, int block_size, int channels);
	block_id pixelToBlock(Block::Pixel_RGBA pixel);
	uint32_t calculateColorDifference(Block::Pixel_RGBA p_one, Block::Pixel_RGBA p_two);

	// Common
	static ImageBundle resizeImage(ImageBundle src, glm::vec3 scale);
	static ImageBundle read_png_image(std::string filepath);
}