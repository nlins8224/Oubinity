#pragma once
#include <cstdint>
#include <vector>
#include "../block/PaletteBlockStorage.h"
#include "../level_of_detail/LevelOfDetail.h"
#include "../block/Block.h"

namespace Block
{

/*
This class divides chunks into sections, each has it's own PaletteBlockStorage.
This is because it's very likely that lower sections of a chunk will contain less variety of blocks.
Separating them will save memory.
*/
class PaletteChunkView
{
public:
	// chunk_sections has to be a power of two.
	PaletteChunkView(LevelOfDetail::LevelOfDetail lod, uint8_t chunk_sections = 4);
	block_id get(glm::ivec3 block_pos);
	void set(glm::ivec3 block_pos, block_id block_type);
	PaletteBlockStorage& choosePalette(glm::ivec3 block_pos);

private:
	std::vector<PaletteBlockStorage> m_palettes;
	LevelOfDetail::LevelOfDetail m_lod;
	uint8_t m_chunk_sections;
};
}

