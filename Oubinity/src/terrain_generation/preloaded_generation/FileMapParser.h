#pragma once
#include <memory>
#include <string>
#include <vector>
#include "../../level_of_detail/LevelOfDetail.h"
#include "../../third_party/stb_image.h"
#include "../TerrainGenerationTypes.h"
#include "glm/glm.hpp"

namespace PreloadedGeneration {
struct ImageBundle {
  int width;
  int height;
  int channels;
  std::unique_ptr<unsigned char[]> image;
};

// Grayscale
HeightMapBundle parsePNGToHeightMaps_8BIT(std::string filepath,
                                          glm::vec3 scale);
HeightMap parsePNGToHeightMap_8BIT(unsigned char* chunk_image, int width,
                                   glm::ivec3 chunk_pos_xz, glm::vec3 scale);
HeightMap increaseHeightMapLodLevel(HeightMap base_height_map,
                                    LevelOfDetail::LevelOfDetail lod);

// Color
BlockMapBundle parsePNGToBlockMaps(std::string filepath, glm::vec3 scale);
BlockMap parsePNGToBlockMap(unsigned char* chunk_image, int image_width,
                            int image_height, glm::ivec3 chunk_pos_xz,
                            int channels);
Block::Pixel_RGBA getPixelRGBA(stbi_uc* block_image, int image_width,
                               int image_height, int x, int z, int block_size,
                               int channels);
block_id pixelToBlock(Block::Pixel_RGBA pixel);
uint32_t calculateColorDifference(Block::Pixel_RGBA p_one,
                                  Block::Pixel_RGBA p_two);
BlockMap increaseBlockMapLodLevel(BlockMap base_height_map,
                                  LevelOfDetail::LevelOfDetail lod);

// Common
static ImageBundle resizeImage(ImageBundle src, glm::vec3 scale);
static ImageBundle read_png_image(std::string filepath);
}  // namespace PreloadedGeneration