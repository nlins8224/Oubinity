#pragma once
#include <array>
#include <unordered_map>
#include "../chunk/ChunkSize.h"

using HeightMap = std::array<std::array<double, CHUNK_SIZE_X>, CHUNK_SIZE_Z>;

using DensityMap = std::array<std::array<std::array<double, CHUNK_SIZE_X>, CHUNK_SIZE_Y>, CHUNK_SIZE_Z>;

using SplineRange = std::unordered_map<int, double>;