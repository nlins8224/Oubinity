#pragma once
#include <vector>
#include "../../chunk/Chunk.h"
#include "../../renderer/ChunkRendererSettings.h"
#include "../TerrainGenerationTypes.h"
#include "HeightMapParser.h"

class PreloadedGenerator
{
public:
	PreloadedGenerator();
	virtual ~PreloadedGenerator() = default;
	HeightMap generateHeightMap(Chunk& chunk);
	void generateLayers(Chunk& chunk, HeightMap height_map);

private:
	glm::ivec3 mapChunkPosToHeightMapPos(glm::ivec3 chunk_pos);
	std::vector<HeightMap> m_height_maps;
};