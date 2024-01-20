#pragma once
#include <array>
#include "../chunk/ChunkSize.h"

using HeightMap = std::array<std::array<double, CHUNK_SIZE>, CHUNK_SIZE>;
