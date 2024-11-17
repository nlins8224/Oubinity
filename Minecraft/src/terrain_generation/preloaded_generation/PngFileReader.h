#pragma once
#include <string>
#include "../../third_party/stb_image.h"
#include "../../loguru.hpp"
#include "../../chunk/ChunkSize.h"
#include "../TerrainGenerationTypes.h"

namespace PreloadedGeneration
{
	inline ImageBundle read_png_image(std::string filepath) {
		int width, height, channels;
		unsigned char* png_image = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
		if (!png_image)
		{
			LOG_F(ERROR, "image loaded under path: %s does not exist", filepath);
		}

		LOG_F(INFO, "PNG Image height: %d", height);
		LOG_F(INFO, "PNG Image width: %d", width);

		int width_mod = width % CHUNK_SIZE;
		if (width_mod != 0)
		{
			width -= width_mod;
			LOG_F(WARNING, "width mod CHUNK_SIZE is: %d, but should be 0, trimmed width to: %d", width_mod, width);
		}
		return { width, height, channels, png_image };
	}
}
