#include "PreloadedGenerator.h"

PreloadedGenerator::PreloadedGenerator()
	: m_height_maps{PreloadedGeneration::parsePNGToHeightMaps("assets/heightmap_64x64.png")},
	 m_block_maps{PreloadedGeneration::parsePNGToBlockMaps("assets/colormap_64x64_v4_2.png")}
{
}

HeightMap PreloadedGenerator::generateHeightMap(glm::ivec3 chunk_pos)
{
	glm::ivec3 chunk_pos_in_heightmap = mapChunkPosToHeightMapPos(chunk_pos);
	return m_height_maps.at(chunk_pos_in_heightmap.x * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS + chunk_pos_in_heightmap.z);
}

BlockMap PreloadedGenerator::generateBlockMap(glm::ivec3 chunk_pos)
{
	glm::ivec3 chunk_pos_in_map = mapChunkPosToHeightMapPos(chunk_pos);
	return m_block_maps.at(chunk_pos_in_map.x * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS + chunk_pos_in_map.z);
}

glm::ivec3 PreloadedGenerator::mapChunkPosToHeightMapPos(glm::ivec3 chunk_pos)
{
	return chunk_pos + ((ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS - 1) / 2);
}

