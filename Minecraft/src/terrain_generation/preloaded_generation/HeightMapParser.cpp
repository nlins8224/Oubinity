#include "HeightMapParser.h"
#include "PngFileReader.h"
#include "../../chunk/Chunk.h"
#include "../../third_party/stb_image.h"
#include "../../loguru.hpp"
#include "../../renderer/ChunkRendererSettings.h"
#include "../TerrainGenerationTypes.h"
#include "MapResizer.h"

namespace PreloadedGeneration
{
	HeightMapBundle parsePNGToHeightMaps_8BIT(std::string filepath, glm::vec3 scale)
	{
		ImageBundle img_bundle = resizeImageGrayscale(read_png_image(filepath), 2048, 2048);
		int height{ img_bundle.height }, width{ img_bundle.width }, channels{ img_bundle.channels };
		unsigned char* png_image{ img_bundle.image };

		std::vector<HeightMap> height_maps{};
		for (int x = 0; x < height; x += CHUNK_SIZE / scale.x) {
			for (int z = 0; z < width; z += CHUNK_SIZE / scale.z) {
				int chunk_offset = x * width + z;
				glm::ivec3 chunk_pos_xz{ x * scale.x, 0, z * scale.z };
				chunk_pos_xz /= CHUNK_SIZE;
				// map from height map coords to chunk pos coords
				chunk_pos_xz -= (ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS - 1) / 2;
				height_maps.push_back(parsePNGToHeightMap_8BIT(png_image + chunk_offset, width, chunk_pos_xz, scale));
			}
		}

		LOG_F(INFO, "height_maps size: %d", height_maps.size());
		return { width, height, height_maps };
	}

	HeightMap parsePNGToHeightMap_8BIT(unsigned char* chunk_image, int width, glm::ivec3 chunk_pos_xz, glm::vec3 scale)
	{
		auto lod = LevelOfDetail::chooseLevelOfDetail({ 0, 0, 0 }, chunk_pos_xz);
		int block_size = 1;
		int block_amount = CHUNK_SIZE;
		int scale_factor_x = static_cast<int>(scale.x);
		int scale_factor_z = static_cast<int>(scale.z);

		HeightMap height_map{};
		for (int x = 0; x < block_amount; x += scale_factor_x) {
			for (int z = 0; z < block_amount; z += scale_factor_z) {
				int surface_height = chunk_image[( (x / scale_factor_x) * block_size * width) + ((z / scale_factor_z) * block_size)] * scale.y;
				for (int i = 0; i < scale_factor_x; i++) {
					for (int j = 0; j < scale_factor_z; j++) {
						height_map[x + i][z + j] = surface_height;
					}
				}
			}
		}

		return height_map;
	}
}