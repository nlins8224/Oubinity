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
	/*
	TODO: currently chunk_sections > 1 has a memory overhead and is not an optimisation, because PaletteBlockStorage needs some
	additional work
	*/
	// chunk_sections has to be a in a form of k^3.
	PaletteChunkView(LevelOfDetail::LevelOfDetail lod, uint8_t chunk_sections = 1);
	block_id get(glm::ivec3 block_pos);
	void set(glm::ivec3 block_pos, block_id block_type);
	void fill(block_id block_type);

private:
	PaletteBlockStorage& choosePalette(glm::ivec3 block_pos);
	std::vector<PaletteBlockStorage> m_palettes;
	LevelOfDetail::LevelOfDetail m_lod;
	uint8_t m_chunk_sections;
};
}

