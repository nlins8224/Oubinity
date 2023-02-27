#pragma once
#include <array>
#include "../chunk/ChunkSize.h"

using HeightMap = std::array<std::array<uint8_t, CHUNK_SIZE_X>, CHUNK_SIZE_Z>;
