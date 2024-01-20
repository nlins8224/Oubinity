#pragma once
#include "../Settings.h"
#include "../chunk/Chunk.h"
#include "../chunk/ChunkSize.h"
#include "procedural_generation/ProceduralGenerator.h";


class TerrainGenerator
{
public:
	TerrainGenerator();
	TerrainGenerator(int world_seed, uint8_t surface_height, uint8_t water_height);
	~TerrainGenerator() = default;
	void generateChunkTerrain(Chunk& chunk);
private:
	HeightMap generateHeightMap(Chunk& chunk);
	void generateLayers(Chunk& chunk, HeightMap height_map);

#if !defined(SETTING_USE_PRELOADED_HEIGHTMAP) || !defined(SETTING_USE_PRELOADED_LAYERS)
#endif

#if defined(SETTING_USE_PRELOADED_HEIGHTMAP) || defined(SETTING_USE_PRELOADED_LAYERS)
	ProceduralGenerator m_procedural_generator;
#endif
};

