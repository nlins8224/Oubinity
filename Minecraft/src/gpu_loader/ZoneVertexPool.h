#pragma once
#include <glad/glad.h>
#include "../chunk/Vertex.h"
#include "../renderer/ChunkRendererSettings.h"
#include "../chunk/ChunkSize.h"
#include "../chunk/Chunk.h"
#include <vector>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <glm/glm.hpp>
#include "../loguru.hpp"

#include "optick.h"

namespace ZonePool {
	static const size_t MAX_BLOCKS_IN_CHUNK = CHUNK_SIZE * CHUNK_SIZE;
	static const size_t FACES_IN_BLOCK = 6;
	static const size_t VERTICES_IN_FACE = 6;
	static const size_t MAX_VERTICES_IN_LARGEST_BUCKET = MAX_BLOCKS_IN_CHUNK * FACES_IN_BLOCK * VERTICES_IN_FACE; // 3 * 3 * 2^12

	using namespace ChunkRendererSettings;
	static const size_t EXTRA_BUFFER_SPACE = 512 * MAX_RENDERED_CHUNKS_IN_Y_AXIS;
	static const size_t MIN_BUCKETS_AMOUNT = 32768 * MAX_RENDERED_CHUNKS_IN_Y_AXIS;
	static const size_t TOTAL_CHUNKS = MAX_RENDERED_CHUNKS_IN_XZ_AXIS * MAX_RENDERED_CHUNKS_IN_XZ_AXIS * MAX_RENDERED_CHUNKS_IN_Y_AXIS;
	static const size_t TOTAL_BUCKETS_AMOUNT = std::max(TOTAL_CHUNKS, MIN_BUCKETS_AMOUNT);

	static const size_t MAX_DAIC_AMOUNT = TOTAL_BUCKETS_AMOUNT;

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

	static size_t calculateTotalVertexAmount()
	{
		size_t vertex_amount = 0;
		LOG_F(INFO, "calculateTotalVertexAmount");

		for (Zone* zone : zones)
		{
			vertex_amount += zone->max_vertices_per_bucket * zone->buckets_amount;
			LOG_F(INFO, "zone %d vertexamount: %d, max_vertices_per_bucket: %d, buckets_amount: %d", zone->level, zone->max_vertices_per_bucket * zone->buckets_amount,  zone->max_vertices_per_bucket, zone->buckets_amount);

		}
		return vertex_amount;
	}

	static size_t calculateBucketAmountInZones()
	{
		size_t buckets_added = 0;
		Zero.buckets_amount = std::pow(LevelOfDetail::One.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS;
		buckets_added += Zero.buckets_amount;
		One.buckets_amount = std::pow(LevelOfDetail::Two.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - buckets_added;
		buckets_added += One.buckets_amount;
		Two.buckets_amount = std::pow(LevelOfDetail::Three.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - buckets_added;
		buckets_added += Two.buckets_amount;
		Three.buckets_amount = std::pow(LevelOfDetail::Four.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - buckets_added;
		buckets_added += Three.buckets_amount;
		Four.buckets_amount = std::pow(LevelOfDetail::Five.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - buckets_added;
		buckets_added += Four.buckets_amount;
		Five.buckets_amount = TOTAL_BUCKETS_AMOUNT - buckets_added;
		buckets_added += Five.buckets_amount;

		// For a short while Zone buffer might overflow, add extra buckets to prevent that
		// Example: Zone is full, there are pending tasks in the queue
		// and "allocate" tasks are preceeding "free" tasks
		Zero.buckets_amount += EXTRA_BUFFER_SPACE;
		One.buckets_amount += EXTRA_BUFFER_SPACE;
		Two.buckets_amount += EXTRA_BUFFER_SPACE;
		Three.buckets_amount += EXTRA_BUFFER_SPACE;
		Four.buckets_amount += EXTRA_BUFFER_SPACE;
		Five.buckets_amount += EXTRA_BUFFER_SPACE;

		buckets_added += zones.size() * EXTRA_BUFFER_SPACE;

		LOG_F(INFO, "buckets amount, Zero: %d", Zero.buckets_amount);
		LOG_F(INFO, "buckets amount, One: %d", One.buckets_amount);
		LOG_F(INFO, "buckets amount, Two: %d", Two.buckets_amount);
		LOG_F(INFO, "buckets amount, Three: %d", Three.buckets_amount);
		LOG_F(INFO, "buckets amount, Four: %d", Four.buckets_amount);
		LOG_F(INFO, "buckets amount, Five: %d", Five.buckets_amount);
		LOG_F(INFO, "buckets_added: %d", buckets_added);
		return buckets_added;
	}

	static void initZones(Vertex* buffer)
	{
		size_t total_vertex_amount = calculateTotalVertexAmount();

		size_t vertices_in_zero_zone = Zero.max_vertices_per_bucket * Zero.buckets_amount;
		Zero.start_offset = 0;
		Zero.end_offset = vertices_in_zero_zone;

		size_t vertices_in_first_zone = One.max_vertices_per_bucket * One.buckets_amount;
		One.start_offset = Zero.end_offset;
		One.end_offset = One.start_offset + vertices_in_first_zone;

		size_t vertices_in_second_zone = Two.max_vertices_per_bucket * Two.buckets_amount;
		Two.start_offset = One.end_offset;
		Two.end_offset = Two.start_offset + vertices_in_second_zone;

		size_t vertices_in_third_zone = Three.max_vertices_per_bucket * Three.buckets_amount;
		Three.start_offset = Two.end_offset;
		Three.end_offset = Three.start_offset + vertices_in_third_zone;

		size_t vertices_in_fourth_zone = Four.max_vertices_per_bucket * Four.buckets_amount;
		Four.start_offset = Three.end_offset;
		Four.end_offset = Four.start_offset + vertices_in_fourth_zone;

		Five.start_offset = Four.end_offset;
		Five.end_offset = total_vertex_amount;

		LOG_F(INFO, "Zero start offset: %d, end offset: %d", Zero.start_offset, Zero.end_offset);
		LOG_F(INFO, "One start offset: %d, end offset: %d", One.start_offset, One.end_offset);
		LOG_F(INFO, "Two start offset: %d, end offset: %d", Two.start_offset, Two.end_offset);
		LOG_F(INFO, "Three start offset: %d, end offset: %d", Three.start_offset, Three.end_offset);
		LOG_F(INFO, "Four start offset: %d, end offset: %d", Four.start_offset, Four.end_offset);
		LOG_F(INFO, "Five start offset: %d, end offset: %d", Five.start_offset, Five.end_offset);

	}

	struct ChunkInfo
	{
		glm::vec4 chunk_pos[MAX_DAIC_AMOUNT];
	};

	struct ChunksLod
	{
		GLuint chunks_lod[MAX_DAIC_AMOUNT];
	};

	struct MeshBucket
	{
		bool _is_free{ true };
		unsigned int _start_offset;
		size_t _id;
	};

	struct DAIC
	{
		DAIC(unsigned int c, unsigned int ic, unsigned int f, unsigned int bi)
		{
			count = c;
			instance_count = ic;
			first = f;
			base_instance = bi;
		}

		unsigned int count;
		unsigned int instance_count;
		unsigned int first;
		unsigned int base_instance;
	};

	// All struct elements in ChunkMetadata share the same ID
	struct ChunkMetadata
	{
		ChunkInfo active_chunk_info;
		ChunksLod active_chunks_lod;
		std::vector<DAIC> active_daics;
	};

	struct VertexPoolStats
	{
		std::array<size_t, 6> max_vertices_occurred;
		std::array<size_t, 6> min_vertices_occurred;
		std::array<size_t, 6> chunks_in_buckets;
	};

	struct pair_hash
	{
		template <class T1, class T2>
		std::size_t operator() (const std::pair<T1, T2>& pair) const
		{
			return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
		}
	};

	class ZoneVertexPool
	{
	public:
		ZoneVertexPool();
		virtual ~ZoneVertexPool();
		void draw();
		void allocate(Chunk& chunk);
		void free(glm::ivec3 chunk_pos);

		void createChunkInfoBuffer();
		void createChunkLodBuffer();

		void memCheck();
		void daicCheck();

	private:
		void initBuckets();
		void createMeshBuffer();
		void updateMeshBuffer(std::vector<Vertex>& mesh, int buffer_offset);
		void formatVBO();
		void waitBuffer();
		void lockBuffer();
		void fastErase(glm::ivec3 chunk_pos);

		MeshBucket* getFirstFreeBucket(int zone_id);
		Zone chooseZone(unsigned int lod_level);
		std::pair<size_t, size_t> getBucketIdFromDAIC(DAIC daic);
		Zone calculateZoneFromDaicStartOffset(DAIC daic);

		GLuint m_vao;
		GLuint m_vbo;
		GLuint m_daicbo;
		GLuint m_chunk_info_ssbo;
		GLuint m_chunks_lod_ssbo;

		Vertex* m_mesh_persistent_buffer;
		GLsync m_sync;

		ChunkMetadata m_chunk_metadata;
		std::unordered_map<glm::ivec3, std::pair<size_t, size_t>> m_chunk_pos_to_bucket_id;
		std::unordered_map<std::pair<size_t, size_t>, size_t, pair_hash> m_bucket_id_to_daic_id;
		VertexPoolStats m_stats;

		std::vector<std::vector<MeshBucket>> m_chunk_buckets;
	};

}