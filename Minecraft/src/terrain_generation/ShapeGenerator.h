#pragma once
#include "NoiseMapTypes.h"
#include "shapes/NoiseGenerator.h"
#include "../block/Block.h"
#include "../chunk/Chunk.h"

namespace std
{
	template<>
	struct hash<glm::ivec2>
	{
		const size_t operator()(const glm::ivec2& vec) const
		{
			size_t res = 17;
			res = res * 31 + std::hash<int>()(vec.x);
			res = res * 31 + std::hash<int>()(vec.y);
			return res;
		};
	};
}

using ChunkHeightMaps = std::unordered_map<glm::ivec2, NoiseMap>;

class ShapeGenerator
{
public:
	ShapeGenerator(int seed);
	~ShapeGenerator() = default;
	void generateSurfaceMap(Chunk& chunk);
	NoiseMap& getSurfaceMap(glm::ivec2 chunk_pos_xz);

private:
	int m_seed;
	ChunkHeightMaps m_surface_maps;
};