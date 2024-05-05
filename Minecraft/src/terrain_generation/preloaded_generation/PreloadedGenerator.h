#pragma once
#include <vector>
#include "../../chunk/Chunk.h"
#include "../../renderer/ChunkRendererSettings.h"
#include "../../Settings.h"
#include "../TerrainGenerationTypes.h"
#include "HeightMapParser.h"
#include "ColorMapParser.h"
#include "../procedural_generation/DecorationGenerator.h"

class PreloadedGenerator
{
public:
	PreloadedGenerator(uint8_t water_height, glm::vec3 scale = Settings::SETTING_SCALE);
	virtual ~PreloadedGenerator() = default;
	HeightMap& getHeightMap(glm::ivec3 chunk_pos);
	BlockMap& getBlockMap(glm::ivec3 chunk_pos);
	void generateTrees(Chunk& chunk);
private:
	HeightMap& getTreeMap(glm::ivec3 chunk_pos);
	TreePresenceMap generateTreePresenceMap(HeightMap& tree_map);
	glm::ivec3 mapChunkPosToHeightMapPos(glm::ivec3 chunk_pos);
	std::vector<HeightMap> m_height_maps;
	std::vector<BlockMap> m_block_maps;
	std::vector<HeightMap> m_tree_maps;
	uint8_t m_water_height;
	DecorationGenerator m_decoration_generator;
};