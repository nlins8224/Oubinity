#pragma once
#include <array>
#include <vector>
#include "../chunk/ChunkSize.h"
#include "../block/Block.h"

using Block::block_id;
using Block::Pixel_RGBA;

// 2D surface maps
using HeightMap = std::array<std::array<double, CHUNK_SIZE>, CHUNK_SIZE>;
using PreloadedHeightMap = std::array<std::array<double, CHUNK_SIZE>, CHUNK_SIZE>;
using ProceduralHeightMap = std::array<std::array<double, CHUNK_SIZE>, CHUNK_SIZE>;
using TreePresenceMap = std::array<std::array<bool, CHUNK_SIZE>, CHUNK_SIZE>;
using ColorMap = std::array<std::array<Pixel_RGBA, CHUNK_SIZE>, CHUNK_SIZE>;
using BlockMap = std::array<std::array<block_id, CHUNK_SIZE>, CHUNK_SIZE>;

struct ImageBundle {
	int width;
	int height;
	int channels;
	unsigned char* image;
};
