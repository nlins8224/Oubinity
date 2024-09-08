#pragma once
#include "../Settings.h"
#include "../chunk/Chunk.h"
#include "../chunk/ChunkSize.h"
#include "procedural_generation/ProceduralGenerator.h";
#include "preloaded_generation/PreloadedGenerator.h"


class TerrainGenerator
{
public:
	TerrainGenerator() = delete;
	TerrainGenerator(int world_seed, uint8_t water_height);
	~TerrainGenerator() = default;
	// true if chunk is visible, false if it's not
	bool generateChunkTerrain(Chunk& chunk);
	void generateChunkTerrain(Chunk& chunk, HeightMap& height_map, bool is_chunk_visible);
	bool generatePreloadedUndergroundLayer(Chunk& chunk, HeightMap& height_map);
	void generateTrees(Chunk& chunk);
	bool isChunkBelowOrAboveSurface(glm::ivec3 chunk_pos, HeightMap& height_map, LevelOfDetail::LevelOfDetail lod);
	HeightMap generateHeightMap(Chunk& chunk);
	HeightMap generateHeightMap(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod);
	bool generateLayers(Chunk& chunk, HeightMap height_map);
	uint8_t getWaterHeight();
private:
	ProceduralGenerator m_procedural_generator;
#if SETTING_USE_PRELOADED_HEIGHTMAP || SETTING_USE_PRELOADED_COLORMAP
	PreloadedGenerator m_preloaded_generator;
#endif
	uint8_t m_water_height;
};

