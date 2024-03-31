#pragma once
#include "PaletteChunkView.h"

Block::PaletteChunkView::PaletteChunkView(LevelOfDetail::LevelOfDetail lod, uint8_t chunk_sections)
	:
	m_lod{lod},
	m_chunk_sections{chunk_sections}
{
	bool can_be_divided_into_sections = lod.block_amount / chunk_sections;
	if (can_be_divided_into_sections)
	{
		m_palettes.resize(chunk_sections, { lod });
	}
	else
	{
		m_palettes.resize(1, { lod });
	}
}

Block::block_id Block::PaletteChunkView::get(glm::ivec3 block_pos)
{
	return choosePalette(block_pos).get(block_pos);
}

void Block::PaletteChunkView::set(glm::ivec3 block_pos, block_id block_type)
{
	choosePalette(block_pos).set(block_pos, block_type);
}

Block::PaletteBlockStorage& Block::PaletteChunkView::choosePalette(glm::ivec3 block_pos)
{
	uint8_t chunklet_length = m_lod.block_amount / std::sqrt(m_chunk_sections);
	uint8_t x_section = block_pos.x / chunklet_length;
	uint8_t z_section = block_pos.z / chunklet_length;
	return m_palettes[x_section * std::sqrt(m_chunk_sections) + z_section];
}
