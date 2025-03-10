#pragma once
#include <array>
#include <vector>
#include "../block/Block.h"
#include "../Settings.h"

using Block::block_id;
using Block::Pixel_RGBA;
using Settings::CHUNK_SIZE;
using Settings::CHUNK_SIZE_PADDING;

// 2D surface maps
using HeightMap = std::array<std::array<double, CHUNK_SIZE_PADDING>, CHUNK_SIZE_PADDING>;
using PreloadedHeightMap = std::array<std::array<double, CHUNK_SIZE_PADDING>, CHUNK_SIZE_PADDING>;

using ProceduralHeightMap =
    std::array<std::array<double, CHUNK_SIZE_PADDING>, CHUNK_SIZE_PADDING>;
using TreePresenceMap = std::array<std::array<bool, CHUNK_SIZE>, CHUNK_SIZE>;
using ColorMap = std::array<std::array<Pixel_RGBA, CHUNK_SIZE_PADDING>, CHUNK_SIZE_PADDING>;
using BlockMap = std::array<std::array<block_id, CHUNK_SIZE_PADDING>, CHUNK_SIZE_PADDING>;

struct HeightMapBundle {
  int world_width;
  int world_height;
  std::vector<HeightMap> maps;
};

struct BlockMapBundle {
  int world_width;
  int world_height;
  std::vector<BlockMap> maps;
};

struct ImageBundle {
  int width;
  int height;
  int channels;
  unsigned char* image;
};