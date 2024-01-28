#include "ColorMapParser.h"

namespace PreloadedGeneration
{

	std::vector<BlockMap> parsePNGToBlockMaps(std::string filepath)
	{
		int width, height, channels;
		unsigned char* png_image = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
		if (!png_image)
		{
			LOG_F(ERROR, "image loaded under path: %s does not exist", filepath);
		}

		LOG_F(INFO, "PNG Image height: %d", height);
		LOG_F(INFO, "PNG Image width: %d", width);
		LOG_F(INFO, "PNG Image Channels: %d", channels);

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

		std::vector<BlockMap> block_maps{};

		int bytes_per_pixel = channels;
		for (int x = 0; x < height; x += CHUNK_SIZE) {
			for (int z = 0; z < width; z += CHUNK_SIZE) {
				int chunk_offset = (x * width + z) * bytes_per_pixel;
				glm::ivec3 chunk_pos_xz{ x, 0, z };
				chunk_pos_xz /= CHUNK_SIZE;
				// map from height map coords to chunk pos coords
				chunk_pos_xz -= (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS - 1) / 2;
				block_maps.push_back(parsePNGToBlockMap(png_image + chunk_offset, width, height, chunk_pos_xz, channels));
			}
		}

		LOG_F(INFO, "block_maps size: %d", block_maps.size());
		return block_maps;
	}

	BlockMap parsePNGToBlockMap(unsigned char* chunk_image, int image_width, int image_height, glm::ivec3 chunk_pos_xz, int channels)
	{
		auto lod = LevelOfDetail::chooseLevelOfDetail({ 0, 0, 0 }, chunk_pos_xz);
		int block_size = lod.block_size;
		int block_amount = lod.block_amount;

		BlockMap block_map{};
		for (int x = 0; x < block_amount; x++) {
			for (int z = 0; z < block_amount; z++) {
				Block::Pixel_RGBA pixel = getPixelRGBA(chunk_image, image_width, image_height, x, z, block_size, channels);
				block_map[x][z] = pixelToBlock(pixel);
			}
		}
		return block_map;
	}

	Block::Pixel_RGBA getPixelRGBA(stbi_uc* block_image, int image_width, int image_height, int x, int z, int block_size, int channels)
	{
		uint8_t byte_per_pixel = channels;
		Block::Pixel_RGBA pixel_rgba;
		stbi_uc* p = block_image + ((x * block_size * image_width + (z * block_size)) * byte_per_pixel);
		pixel_rgba.r = p[0];
		pixel_rgba.g = p[1];
		pixel_rgba.b = p[2];
		pixel_rgba.a = 0;
		//LOG_F(INFO, "Pixel channels=%d, offset=%d r=%d, g=%d, b=%d, a=%d", channels, offset, pixel_rgba.r, pixel_rgba.g, pixel_rgba.b, pixel_rgba.a);
		return pixel_rgba;
	}

	block_id pixelToBlock(Block::Pixel_RGBA pixel)
	{
		uint32_t closest_difference = UINT32_MAX;
		block_id closest_block = Block::AIR;

		for (int block = Block::AIR; block != Block::AMOUNT; block++) {
			Block::Pixel_RGBA block_color = Block::getBlockColor(static_cast<Block::block_id>(block));
			uint32_t color_difference = calculateColorDifference(pixel, block_color);
			if (color_difference < closest_difference) {
				closest_difference = color_difference;
				closest_block = static_cast<Block::block_id>(block);
			}
		}
		return closest_block;
	}

	uint32_t calculateColorDifference(Block::Pixel_RGBA p_one, Block::Pixel_RGBA p_two)
	{		
		// squared euclidean distance for now
		return (p_one.r - p_two.r) * (p_one.r - p_two.r)
			+ (p_one.g - p_two.g) * (p_one.g - p_two.g)
			+ (p_one.b - p_two.b) * (p_one.b - p_two.b);
	}
}