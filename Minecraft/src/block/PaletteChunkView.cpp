#pragma once
#include "PaletteChunkView.h"

Block::PaletteChunkView::PaletteChunkView(LevelOfDetail::LevelOfDetail lod, uint8_t chunk_sections)
	:
	m_lod{lod},
	m_chunk_sections{chunk_sections}
{
	uint8_t section_length = lod.block_amount / std::cbrt(m_chunk_sections);
	if (section_length > 0)
	{
		m_palettes.resize(chunk_sections, { section_length });
	}
	else
	{
		m_palettes.resize(1, { (uint8_t)lod.block_amount });
	}
}

Block::block_id Block::PaletteChunkView::get(glm::ivec3 block_pos)
{
	uint8_t section_length = m_lod.block_amount / std::cbrt(m_chunk_sections);
	glm::ivec3 block_pos_offset = block_pos % glm::ivec3(section_length);
	return choosePalette(block_pos).get(block_pos_offset);
}

void Block::PaletteChunkView::set(glm::ivec3 block_pos, block_id block_type)
{
	uint8_t section_length = m_lod.block_amount / std::cbrt(m_chunk_sections);
	glm::ivec3 block_pos_offset = block_pos % glm::ivec3(section_length);
	choosePalette(block_pos).set(block_pos_offset, block_type);
}

void Block::PaletteChunkView::fill(block_id block_type)
{
	for (auto& section : m_palettes)
	{
		section.fill(block_type);
	}
}

Block::PaletteBlockStorage& Block::PaletteChunkView::choosePalette(glm::ivec3 block_pos)
{
	uint8_t sections_cbrt = std::cbrt(m_chunk_sections);
	uint8_t chunklet_length = m_lod.block_amount / sections_cbrt;
	glm::ivec3 axis_section = block_pos / glm::ivec3(chunklet_length);
	return m_palettes[axis_section.x * sections_cbrt * sections_cbrt + axis_section.y * sections_cbrt + axis_section.z];
}
