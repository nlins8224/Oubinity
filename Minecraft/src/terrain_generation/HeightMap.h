#pragma once
#include <array>
#include "../chunk/ChunkSize.h"

using height_map = std::array<std::array<uint8_t, CHUNK_SIZE_X>, CHUNK_SIZE_Z>;
