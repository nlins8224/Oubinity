#include "PreloadedGenerator.h"

PreloadedGenerator::PreloadedGenerator()
	: m_height_maps{PreloadedGeneration::parsePNGToHeightMaps("assets/heightmap_64x64_v4.png")}
{
}

HeightMap PreloadedGenerator::generateHeightMap(Chunk& chunk)
{
	glm::ivec3 chunk_pos_in_heightmap = mapChunkPosToHeightMapPos(chunk.getPos());
	return m_height_maps.at(chunk_pos_in_heightmap.x * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS + chunk_pos_in_heightmap.z);
}

glm::ivec3 PreloadedGenerator::mapChunkPosToHeightMapPos(glm::ivec3 chunk_pos)
{
	return chunk_pos + ((ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS - 1) / 2);
}
