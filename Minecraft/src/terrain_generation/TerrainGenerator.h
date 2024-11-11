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
	void generateChunkTerrain(Chunk& chunk, ProceduralHeightMap& height_map, bool is_chunk_visible);
	void generateTrees(Chunk& chunk);
	bool isChunkBelowOrAboveSurface(Chunk& chunk, const ProceduralHeightMap& height_map);
	bool isChunkBelowOrAboveSurface(glm::ivec3 chunk_pos, const ProceduralHeightMap& height_map, LevelOfDetail::LevelOfDetail lod);
	ProceduralHeightMap generateProceduralHeightMap(Chunk& chunk);
	ProceduralHeightMap generateProceduralHeightMap(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod);
	bool generateProceduralLayers(Chunk& chunk, ProceduralHeightMap height_map);
	uint8_t getWaterHeight();
#if SETTING_USE_PRELOADED_HEIGHTMAP || SETTING_USE_PRELOADED_COLORMAP
	bool generatePreloadedUndergroundLayer(Chunk& chunk, HeightMap& height_map);
	PreloadedHeightMap generatePreloadedHeightMap(glm::ivec3 chunk_pos);
#endif

#if SETTING_USE_PRELOADED_COLORMAP
	bool generatePreloadedLayers(Chunk& chunk, HeightMap height_map);
#endif

private:
	ProceduralGenerator m_procedural_generator;
#if SETTING_USE_PRELOADED_HEIGHTMAP || SETTING_USE_PRELOADED_COLORMAP
	PreloadedGenerator m_preloaded_generator;
#endif
	uint8_t m_water_height;
};

