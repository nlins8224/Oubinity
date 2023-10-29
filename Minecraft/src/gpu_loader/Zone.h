//#pragma once
//#include "../chunk/ChunkSize.h"
//#include "../chunk/Vertex.h"
//#include "../renderer/ChunkRendererSettings.h"
//
//namespace ZonePool
//{
//	static const size_t MAX_BLOCKS_IN_CHUNK = CHUNK_SIZE * CHUNK_SIZE;
//	static const size_t FACES_IN_BLOCK = 6;
//	static const size_t VERTICES_IN_FACE = 6;
//	static const size_t MAX_VERTICES_IN_LARGEST_BUCKET = MAX_BLOCKS_IN_CHUNK * FACES_IN_BLOCK * VERTICES_IN_FACE; // 3 * 3 * 2^12
//
//	using namespace ChunkRendererSettings;
//	static const size_t EXTRA_BUFFER_SPACE = 128;
//	static const size_t TOTAL_BUCKETS_AMOUNT = 
//		MAX_RENDERED_CHUNKS_IN_XZ_AXIS * MAX_RENDERED_CHUNKS_IN_XZ_AXIS * MAX_RENDERED_CHUNKS_IN_Y_AXIS + EXTRA_BUFFER_SPACE;
//
//	struct Zone
//	{
//		size_t level;
//		size_t max_vertices_per_bucket;
//		size_t buckets_amount; // calculated at ZoneVertexPool init, based on LOD
//		Vertex* start; // initialized at ZoneVertexPool init
//		Vertex* end; // initialized at ZoneVertexPool init
//		size_t start_offset; // ID of start Vertex
//		size_t end_offset; // ID of end Vertex
//	};
//
//	static Zone One
//	{
//		.level{1},
//		.max_vertices_per_bucket{MAX_VERTICES_IN_LARGEST_BUCKET},
//		.buckets_amount{0},
//		.start{nullptr},
//		.end{nullptr},
//		.start_offset{0},
//		.end_offset{0}
//	};
//
//	static Zone Two
//	{
//		.level{2},
//		.max_vertices_per_bucket{MAX_VERTICES_IN_LARGEST_BUCKET / 2},
//		.buckets_amount{0},
//		.start{nullptr},
//		.end{nullptr},
//		.start_offset{0},
//		.end_offset{0}
//	};
//
//	static Zone Three
//	{
//		.level{3},
//		.max_vertices_per_bucket{MAX_VERTICES_IN_LARGEST_BUCKET / 4},
//		.buckets_amount{0},
//		.start{nullptr},
//		.end{nullptr},
//		.start_offset{0},
//		.end_offset{0}
//	};
//
//	static Zone Four
//	{
//		.level{4},
//		.max_vertices_per_bucket{MAX_VERTICES_IN_LARGEST_BUCKET / 8},
//		.buckets_amount{0},
//		.start{nullptr},
//		.end{nullptr},
//		.start_offset{0},
//		.end_offset{0}
//	};
//
//	static Zone Five
//	{
//		.level{5},
//		.max_vertices_per_bucket{MAX_VERTICES_IN_LARGEST_BUCKET / 16},
//		.buckets_amount{0},
//		.start{nullptr},
//		.end{nullptr},
//		.start_offset{0},
//		.end_offset{0}
//	};
//
//	static Zone Six
//	{
//		.level{6},
//		.max_vertices_per_bucket{MAX_VERTICES_IN_LARGEST_BUCKET / 32},
//		.buckets_amount{0},
//		.start{nullptr},
//		.end{nullptr},
//		.start_offset{0},
//		.end_offset{0}
//
//	};
//
//	static std::vector<Zone*> zones
//	{
//		{&One},
//		{&Two},
//		{&Three},
//		{&Four},
//		{&Five},
//		{&Six}
//	};
//
//	static size_t calculateTotalVertexAmount()
//	{
//		size_t vertex_amount = 0;
//		for (Zone* zone : zones)
//		{
//			vertex_amount += zone->max_vertices_per_bucket * zone->buckets_amount;
//		}
//		return vertex_amount;
//	}
//
//	static void initZones(Vertex* buffer)
//	{
//		size_t vertex_amount = calculateTotalVertexAmount();
//
//		// TODO: should be LOD dependent
//		// Maybe additional "swap space" will be needed
//		One.start = buffer;
//		One.end = buffer + (vertex_amount / 2);
//		One.start_offset = 0;
//		One.end_offset = vertex_amount / 2;
//
//		Two.start = One.end;
//		Two.end = Two.start + (vertex_amount / 4);
//		Two.start_offset = One.end_offset;
//		Two.end_offset = Two.start_offset + (vertex_amount / 4);
//
//		Three.start = Two.end;
//		Three.end = Three.start + (vertex_amount / 8);
//		Three.start_offset = Two.end_offset;
//		Three.end_offset = Two.start_offset + (vertex_amount / 8);
//
//		Four.start = Three.end;
//		Four.end = Four.start + (vertex_amount / 16);
//		Four.start_offset = Three.end_offset;
//		Four.end_offset = Four.start_offset + (vertex_amount / 16);
//
//		Five.start = Four.end;
//		Five.end = Five.start + (vertex_amount / 32);
//		Five.start_offset = Four.end_offset;
//		Five.end_offset = Five.start_offset + (vertex_amount / 32);
//
//		Six.start = Five.end;
//		Six.end = buffer + vertex_amount;
//		Six.start_offset = Five.end_offset;
//		Six.end_offset = vertex_amount;
//	}
//
//	static Zone chooseZone(size_t vertices_amount)
//	{
//		Zone target_zone = Six;
//		for (int i = zones.size() - 1; i >= 0; i--)
//		{
//			if (vertices_amount < zones[i]->max_vertices_per_bucket)
//			{
//				target_zone = *zones[i];
//			}
//		}
//		return target_zone;
//	}
//}