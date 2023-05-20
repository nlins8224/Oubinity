#pragma once
#include <array>
#include <unordered_map>
#include "../chunk/ChunkSize.h"

using NoiseMap = std::array<std::array<double, CHUNK_SIZE>, CHUNK_SIZE>;

using DensityMap = std::array<std::array<std::array<double, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_SIZE>;

using SplineRange = std::unordered_map<int, double>;