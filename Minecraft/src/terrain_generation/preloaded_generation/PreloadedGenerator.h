#pragma once
#include <vector>
#include "../../chunk/Chunk.h"
#include "../../renderer/ChunkRendererSettings.h"
#include "../TerrainGenerationTypes.h"
#include "HeightMapParser.h"
#include "ColorMapParser.h"

class PreloadedGenerator
{
public:
	PreloadedGenerator();
	virtual ~PreloadedGenerator() = default;
	HeightMap generateHeightMap(const Chunk& chunk);
	BlockMap generateBlockMap(const Chunk& chunk);

private:
	glm::ivec3 mapChunkPosToHeightMapPos(glm::ivec3 chunk_pos);
	std::vector<HeightMap> m_height_maps;
	std::vector<BlockMap> m_block_maps;
};