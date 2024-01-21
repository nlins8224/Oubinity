#pragma once
#include "../Settings.h"
#include "../chunk/Chunk.h"
#include "../chunk/ChunkSize.h"
#include "procedural_generation/ProceduralGenerator.h";
#include "preloaded_generation/PreloadedGenerator.h"


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

#if SETTING_USE_PRELOADED_HEIGHTMAP == 0 || SETTING_USE_PRELOADED_LAYERS == 0
	ProceduralGenerator m_procedural_generator;
#endif

#if SETTING_USE_PRELOADED_HEIGHTMAP || SETTING_USE_PRELOADED_LAYERS
	PreloadedGenerator m_preloaded_generator;
#endif

};

