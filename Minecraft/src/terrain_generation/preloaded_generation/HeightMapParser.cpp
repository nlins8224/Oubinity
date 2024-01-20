#include "HeightMapParser.h"

namespace HeightMapParser
{
	std::vector<HeightMap> parsePNGToHeightMaps(std::string filepath)
	{
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

		int height_mod = height % CHUNK_SIZE;
		if (height_mod != 0)
		{
			height -= height_mod;
			LOG_F(WARNING, "height mod CHUNK_SIZE is: %d, but should be 0, trimmed height to: %d.", height_mod, height);
		}

		std::vector<HeightMap> height_maps{};
		for (int i = 0; i < height; i += CHUNK_SIZE) {
			for (int j = 0; j < width; j += CHUNK_SIZE) {
				LOG_F(INFO, "%d", png_image[i * width + j]);
				height_maps.push_back(parsePNGToHeightMap(png_image, i, j));
			}
		}

		LOG_F(INFO, "height_maps size: %d", height_maps.size());
		return height_maps;
	}

	HeightMap parsePNGToHeightMap(unsigned char* png_image, int height_offset, int width_offset)
	{
		HeightMap height_map{};
		for (int i = height_offset; i < height_offset + CHUNK_SIZE; i++) {
			for (int j = width_offset; j < width_offset + CHUNK_SIZE; j++) {
				height_map[i % CHUNK_SIZE][j % CHUNK_SIZE] = png_image[i * width_offset + j];
			}
		}
		return height_map;
	}


}