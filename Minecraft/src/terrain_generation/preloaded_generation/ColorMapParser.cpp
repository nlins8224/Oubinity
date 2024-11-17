#include "ColorMapParser.h"
#include "../../chunk/Chunk.h"
#include "PngFileReader.h"
#include "../../loguru.hpp"
#include "../../renderer/ChunkRendererSettings.h"
#include "../../block/Block.h"
#include "../TerrainGenerationTypes.h"

namespace PreloadedGeneration
{
	BlockMapBundle parsePNGToBlockMaps(std::string filepath, glm::vec3 scale)
	{
		ImageBundle img_bundle = read_png_image(filepath);
		int height{ img_bundle.height }, width{ img_bundle.width }, channels{ img_bundle.channels };
		unsigned char* png_image{ img_bundle.image };
		std::vector<BlockMap> block_maps{};

		int chunks_in_blockmap_xz = width / CHUNK_SIZE;

		int bytes_per_pixel = channels;
		for (int x = 0; x < height; x += CHUNK_SIZE / scale.x) {
			for (int z = 0; z < width; z += CHUNK_SIZE / scale.z) {
				int chunk_offset = (x * width + z) * bytes_per_pixel;
				glm::ivec3 chunk_pos_xz{ x * scale.x, 0, z * scale.z };
				chunk_pos_xz /= CHUNK_SIZE;
				// map from height map coords to chunk pos coords
				chunk_pos_xz -= (chunks_in_blockmap_xz - 1) / 2;
				block_maps.push_back(parsePNGToBlockMap(png_image + chunk_offset, width, height, chunk_pos_xz, channels, scale));
			}
		}

		LOG_F(INFO, "block_maps size: %d", block_maps.size());
		return { width, height, block_maps };
	}

	BlockMap parsePNGToBlockMap(unsigned char* chunk_image, int image_width, int image_height, glm::ivec3 chunk_pos_xz, int channels, glm::vec3 scale)
	{
		auto lod = LevelOfDetail::chooseLevelOfDetail({ 0, 0, 0 }, chunk_pos_xz);
		int block_size = 1;
		int block_amount = CHUNK_SIZE;
		int scale_factor_x = static_cast<int>(scale.x);
		int scale_factor_z = static_cast<int>(scale.z);

		BlockMap block_map{};
		for (int x = 0; x < block_amount; x += scale_factor_x) {
			for (int z = 0; z < block_amount; z += scale_factor_z) {
				Block::Pixel_RGBA pixel = getPixelRGBA(chunk_image, image_width, image_height, (x / scale_factor_x), (z / scale_factor_z), block_size, channels);
				for (int i = 0; i < scale_factor_x; i++) {
					for (int j = 0; j < scale_factor_z; j++) {
						block_map[x + i][z + j] = pixelToBlock(pixel);
					}
				}
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