#pragma once
#include <vector>
#include "../../chunk/Chunk.h"
#include "../../renderer/ChunkRendererSettings.h"
#include "../../Settings.h"
#include "../TerrainGenerationTypes.h"
#include "HeightMapParser.h"
#include "ColorMapParser.h"

class PreloadedGenerator
{
public:
	PreloadedGenerator(glm::vec3 scale = Settings::SETTING_SCALE);
	virtual ~PreloadedGenerator() = default;
	HeightMap generateHeightMap(glm::ivec3 chunk_pos);
	BlockMap generateBlockMap(glm::ivec3 chunk_pos);

private:
	glm::ivec3 mapChunkPosToHeightMapPos(glm::ivec3 chunk_pos);
	std::vector<HeightMap> m_height_maps;
	std::vector<BlockMap> m_block_maps;
};