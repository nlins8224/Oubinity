#include "FileMapParser.h"
#include "../../block/Block.h"
#include "../../chunk/Chunk.h"
#include "../../loguru.hpp"
#include "../TerrainGenerationTypes.h"

namespace PreloadedGeneration {
HeightMapBundle parsePNGToHeightMaps_8BIT(std::string filepath,
                                          glm::vec3 scale) {
  ImageBundle img_bundle = resizeImage(read_png_image(filepath), scale);
  int height{img_bundle.height}, width{img_bundle.width},
      channels{img_bundle.channels};
  std::unique_ptr<unsigned char[]> png_image{std::move(img_bundle.image)};

  int chunks_in_heightmap_xz = width / CHUNK_SIZE;

  std::vector<HeightMap> height_maps{};
  for (int x = 0; x < height; x += CHUNK_SIZE) {
    for (int z = 0; z < width; z += CHUNK_SIZE) {
      int chunk_offset = x * width + z;
      glm::ivec3 chunk_pos_xz{x, 0, z};
      chunk_pos_xz /= CHUNK_SIZE;
      // map from height map coords to chunk pos coords
      chunk_pos_xz -= (chunks_in_heightmap_xz - 1) / 2;
      height_maps.push_back(parsePNGToHeightMap_8BIT(
          png_image.get() + chunk_offset, width, chunk_pos_xz, scale));
    }
  }

  LOG_F(INFO, "height_maps size: %d", height_maps.size());
  return {width, height, height_maps};
}

HeightMap parsePNGToHeightMap_8BIT(unsigned char* chunk_image, int width,
                                   glm::ivec3 chunk_pos_xz, glm::vec3 scale) {
  auto lod = LevelOfDetail::chooseLevelOfDetail({0, 0, 0}, chunk_pos_xz);
  int block_size = 1;
  int block_amount = CHUNK_SIZE;
  HeightMap height_map{};
  for (int x = 0; x < block_amount; x++) {
    for (int z = 0; z < block_amount; z++) {
      int surface_height =
          chunk_image[(x * block_size * width) + (z * block_size)] * scale.y;
      height_map[x][z] = surface_height;
    }
  }

  return height_map;
}

BlockMapBundle parsePNGToBlockMaps(std::string filepath, glm::vec3 scale) {
  ImageBundle img_bundle = resizeImage(read_png_image(filepath), scale);
  int height{img_bundle.height}, width{img_bundle.width},
      channels{img_bundle.channels};
  std::unique_ptr<unsigned char[]> png_image{std::move(img_bundle.image)};
  std::vector<BlockMap> block_maps{};

  int chunks_in_blockmap_xz = width / CHUNK_SIZE;

  int bytes_per_pixel = channels;
  for (int x = 0; x < height; x += CHUNK_SIZE) {
    for (int z = 0; z < width; z += CHUNK_SIZE) {
      int chunk_offset = (x * width + z) * bytes_per_pixel;
      glm::ivec3 chunk_pos_xz{x, 0, z};
      chunk_pos_xz /= CHUNK_SIZE;
      // map from height map coords to chunk pos coords
      chunk_pos_xz -= (chunks_in_blockmap_xz - 1) / 2;
      block_maps.push_back(parsePNGToBlockMap(png_image.get() + chunk_offset,
                                              width, height, chunk_pos_xz,
                                              channels));
    }
  }

  LOG_F(INFO, "block_maps size: %d", block_maps.size());
  return {width, height, block_maps};
}

BlockMap parsePNGToBlockMap(unsigned char* chunk_image, int image_width,
                            int image_height, glm::ivec3 chunk_pos_xz,
                            int channels) {
  auto lod = LevelOfDetail::chooseLevelOfDetail({0, 0, 0}, chunk_pos_xz);
  int block_size = 1;
  int block_amount = CHUNK_SIZE;

  BlockMap block_map{};
  for (int x = 0; x < block_amount; x++) {
    for (int z = 0; z < block_amount; z++) {
      Block::Pixel_RGBA pixel = getPixelRGBA(
          chunk_image, image_width, image_height, x, z, block_size, channels);
      block_map[x][z] = pixelToBlock(pixel);
    }
  }
  return block_map;
}

Block::Pixel_RGBA getPixelRGBA(stbi_uc* block_image, int image_width,
                               int image_height, int x, int z, int block_size,
                               int channels) {
  uint8_t byte_per_pixel = channels;
  Block::Pixel_RGBA pixel_rgba;
  stbi_uc* p =
      block_image +
      ((x * block_size * image_width + (z * block_size)) * byte_per_pixel);
  pixel_rgba.r = p[0];
  pixel_rgba.g = p[1];
  pixel_rgba.b = p[2];
  pixel_rgba.a = 0;
  return pixel_rgba;
}

block_id pixelToBlock(Block::Pixel_RGBA pixel) {
  uint32_t closest_difference = UINT32_MAX;
  block_id closest_block = Block::AIR;

  for (int block = Block::AIR; block != Block::AMOUNT; block++) {
    Block::Pixel_RGBA block_color =
        Block::getBlockColor(static_cast<Block::block_id>(block));
    uint32_t color_difference = calculateColorDifference(pixel, block_color);
    if (color_difference < closest_difference) {
      closest_difference = color_difference;
      closest_block = static_cast<Block::block_id>(block);
    }
  }
  return closest_block;
}

uint32_t calculateColorDifference(Block::Pixel_RGBA p_one,
                                  Block::Pixel_RGBA p_two) {
  // squared euclidean distance
  return (p_one.r - p_two.r) * (p_one.r - p_two.r) +
         (p_one.g - p_two.g) * (p_one.g - p_two.g) +
         (p_one.b - p_two.b) * (p_one.b - p_two.b);
}

static unsigned char bilinearInterpolate(const unsigned char* src,
                                         int src_width, int src_height, float x,
                                         float z, int channel) {
  int x1 = std::floor(x);
  int z1 = std::floor(z);
  int x2 = std::min(x1 + 1, src_width - 1);
  int z2 = std::min(z1 + 1, src_height - 1);

  float a = x - x1;
  float b = z - z1;

  auto get_pixel = [&](int px, int pz) -> unsigned char {
    return src[(px * src_width + pz) * channel];
  };

  unsigned char p1 = get_pixel(x1, z1);
  unsigned char p2 = get_pixel(x2, z1);
  unsigned char p3 = get_pixel(x1, z2);
  unsigned char p4 = get_pixel(x2, z2);

  return static_cast<unsigned char>((1 - a) * (1 - b) * p1 + a * (1 - b) * p2 +
                                    (1 - a) * b * p3 + a * b * p4);
}

static ImageBundle resizeImage(ImageBundle src, glm::vec3 scale) {
  int dst_width = scale.x * src.width;
  int dst_height = scale.z * src.height;
  ImageBundle dst_img{
      .width = dst_width,
      .height = dst_height,
      .channels = 1,  // parse channels linearly, even if it's RGB
      .image = std::make_unique<unsigned char[]>(dst_height * dst_width)};

  dst_img.height = dst_height;
  dst_img.width = dst_width;

  float x_scale = static_cast<float>(src.width) / dst_width;
  float z_scale = static_cast<float>(src.height) / dst_height;

  for (int x = 0; x < dst_width; ++x) {
    for (int z = 0; z < dst_height; ++z) {
      float src_x = x * x_scale;
      float src_z = z * z_scale;
      dst_img.image[x * dst_width + z] = bilinearInterpolate(
          src.image.get(), src.width, src.height, src_x, src_z, src.channels);
    }
  }

  return dst_img;
}

static ImageBundle read_png_image(std::string filepath) {
  int width, height, channels;
  unsigned char* png_image =
      stbi_load(filepath.c_str(), &width, &height, &channels, 0);
  if (!png_image) {
    LOG_F(ERROR, "image loaded under path: %s does not exist", filepath);
  }

  LOG_F(INFO, "Image height: %d", height);
  LOG_F(INFO, "Image width: %d", width);
  LOG_F(INFO, "Image channels: %d", channels);

  int width_mod = width % CHUNK_SIZE;
  if (width_mod != 0) {
    width -= width_mod;
    LOG_F(WARNING,
          "width mod CHUNK_SIZE is: %d, but should be 0, trimmed width to: %d",
          width_mod, width);
  }
  return {width, height, channels, std::unique_ptr<unsigned char[]>(png_image)};
}
}  // namespace PreloadedGeneration