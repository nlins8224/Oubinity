#include "HeightMapParser.h"

namespace PreloadedGeneration
{
	std::vector<HeightMap> parsePNGToHeightMaps_8BIT(std::string filepath, double y_scale)
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
		for (int x = 0; x < height; x += CHUNK_SIZE) {
			for (int z = 0; z < width; z += CHUNK_SIZE) {
				int chunk_offset = x * width + z;
				glm::ivec3 chunk_pos_xz{ x, 0, z };
				chunk_pos_xz /= CHUNK_SIZE;
				// map from height map coords to chunk pos coords
				chunk_pos_xz -= (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS - 1) / 2;
				height_maps.push_back(parsePNGToHeightMap_8BIT(png_image + chunk_offset, width, chunk_pos_xz, y_scale));
			}
		}

		LOG_F(INFO, "height_maps size: %d", height_maps.size());
		return height_maps;
	}

	std::vector<HeightMap> parsePNGToHeightMaps_16BIT(std::string filepath, double y_scale)
	{
		int width, height, channels;
		uint16_t* png_image = stbi_load_16(filepath.c_str(), &width, &height, &channels, 0);
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
		for (int x = 0; x < height; x += CHUNK_SIZE) {
			for (int z = 0; z < width; z += CHUNK_SIZE) {
				int chunk_offset = x * width + z;
				glm::ivec3 chunk_pos_xz{ x, 0, z };
				chunk_pos_xz /= CHUNK_SIZE;
				// map from height map coords to chunk pos coords
				chunk_pos_xz -= (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS - 1) / 2;
				height_maps.push_back(parsePNGToHeightMap_16BIT(png_image + chunk_offset, width, chunk_pos_xz, y_scale));
			}
		}

		LOG_F(INFO, "height_maps size: %d", height_maps.size());
		return height_maps;
	}

	HeightMap parsePNGToHeightMap_8BIT(unsigned char* chunk_image, int width, glm::ivec3 chunk_pos_xz, double y_scale)
	{
		auto lod = LevelOfDetail::chooseLevelOfDetail({ 0, 0, 0 }, chunk_pos_xz);
		int block_size = lod.block_size;
		int block_amount = lod.block_amount;

		HeightMap height_map{};
		for (int x = 0; x < block_amount; x++) {
			for (int z = 0; z < block_amount; z++) {
				height_map[x][z] = chunk_image[(x * block_size * width) + (z * block_size)] * y_scale;
			}
		}

		return height_map;
	}

	HeightMap parsePNGToHeightMap_16BIT(uint16_t* chunk_image, int width, glm::ivec3 chunk_pos_xz, double y_scale)
	{
		auto lod = LevelOfDetail::chooseLevelOfDetail({ 0, 0, 0 }, chunk_pos_xz);
		int block_size = lod.block_size;
		int block_amount = lod.block_amount;

		HeightMap height_map{};
		for (int x = 0; x < block_amount; x++) {
			for (int z = 0; z < block_amount; z++) {
				height_map[x][z] = chunk_image[(x * block_size * width) + (z * block_size)] * y_scale;
			}
		}

		return height_map;
	}
}