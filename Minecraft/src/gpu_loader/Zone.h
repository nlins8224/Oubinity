#pragma once
#include <vector>
#include "../chunk/ChunkSize.h"
#include "../chunk/Vertex.h"
#include "../renderer/ChunkRendererSettings.h"

namespace VertexPool
{
	struct Zone
	{
		size_t level;
		size_t max_vertices_per_bucket;
		size_t buckets_amount; // calculated at ZoneVertexPool init, based on LOD
		size_t start_offset; // ID of start Vertex
		size_t end_offset; // ID of end Vertex
	};

	static Zone Zero
	{
		.level{0},
		.max_vertices_per_bucket{40000},
		.buckets_amount{0},
		.start_offset{0},
		.end_offset{0}
	};

	static Zone One
	{
		.level{1},
		.max_vertices_per_bucket{40000 / 2},
		.buckets_amount{0},
		.start_offset{0},
		.end_offset{0}
	};

	static Zone Two
	{
		.level{2},
		.max_vertices_per_bucket{40000 / 4},
		.buckets_amount{0},
		.start_offset{0},
		.end_offset{0}
	};

	static Zone Three
	{
		.level{3},
		.max_vertices_per_bucket{40000 / 8},
		.buckets_amount{0},
		.start_offset{0},
		.end_offset{0}
	};

	static Zone Four
	{
		.level{4},
		.max_vertices_per_bucket{40000 / 16},
		.buckets_amount{0},
		.start_offset{0},
		.end_offset{0}
	};

	static Zone Five
	{
		.level{5},
		.max_vertices_per_bucket{40000 / 32},
		.buckets_amount{0},
		.start_offset{0},
		.end_offset{0}

	};

	static std::vector<Zone*> zones
	{
		{&Zero},
		{&One},
		{&Two},
		{&Three},
		{&Four},
		{&Five}
	};
}