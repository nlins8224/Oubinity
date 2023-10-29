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

	static const size_t EXTRA_BUFFER_SPACE = 128;
	static const size_t MIN_BUCKETS_AMOUNT = 32768;
	using namespace ChunkRendererSettings;
	static const size_t TOTAL_BUCKETS_AMOUNT = std::max(
		MAX_RENDERED_CHUNKS_IN_XZ_AXIS * MAX_RENDERED_CHUNKS_IN_XZ_AXIS * MAX_RENDERED_CHUNKS_IN_Y_AXIS + EXTRA_BUFFER_SPACE, MIN_BUCKETS_AMOUNT);

	static const size_t MAX_DAIC_AMOUNT = TOTAL_BUCKETS_AMOUNT;

	struct Zone
	{
		size_t level;
		size_t max_vertices_per_bucket;
		size_t buckets_amount; // calculated at ZoneVertexPool init, based on LOD
		Vertex* start; // initialized at ZoneVertexPool init
		Vertex* end; // initialized at ZoneVertexPool init
		size_t start_offset; // ID of start Vertex
		size_t end_offset; // ID of end Vertex
	};

	static Zone Zero
	{
		.level{0},
		.max_vertices_per_bucket{28000},
		.buckets_amount{0},
		.start{nullptr},
		.end{nullptr},
		.start_offset{0},
		.end_offset{0}
	};

	static Zone One
	{
		.level{1},
		.max_vertices_per_bucket{20000},
		.buckets_amount{0},
		.start{nullptr},
		.end{nullptr},
		.start_offset{0},
		.end_offset{0}
	};

	static Zone Two
	{
		.level{2},
		.max_vertices_per_bucket{8000},
		.buckets_amount{0},
		.start{nullptr},
		.end{nullptr},
		.start_offset{0},
		.end_offset{0}
	};

	static Zone Three
	{
		.level{3},
		.max_vertices_per_bucket{2000},
		.buckets_amount{0},
		.start{nullptr},
		.end{nullptr},
		.start_offset{0},
		.end_offset{0}
	};

	static Zone Four
	{
		.level{4},
		.max_vertices_per_bucket{1000},
		.buckets_amount{0},
		.start{nullptr},
		.end{nullptr},
		.start_offset{0},
		.end_offset{0}
	};

	static Zone Five
	{
		.level{5},
		.max_vertices_per_bucket{500},
		.buckets_amount{0},
		.start{nullptr},
		.end{nullptr},
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
		// Maybe additional "swap space" will be needed

		size_t buckets_added = 0;
		Zero.buckets_amount = std::pow(LevelOfDetail::One.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS + EXTRA_BUFFER_SPACE;
		buckets_added += Zero.buckets_amount;
		One.buckets_amount = std::pow(LevelOfDetail::Two.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - buckets_added + 2 * EXTRA_BUFFER_SPACE;
		buckets_added += One.buckets_amount;
		Two.buckets_amount = std::pow(LevelOfDetail::Three.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - buckets_added + 3 * EXTRA_BUFFER_SPACE;
		buckets_added += Two.buckets_amount;
		Three.buckets_amount = std::pow(LevelOfDetail::Four.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - buckets_added + 4 * EXTRA_BUFFER_SPACE;
		buckets_added += Three.buckets_amount;
		Four.buckets_amount = std::pow(LevelOfDetail::Five.draw_distance, 2) * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS - buckets_added + 5 * EXTRA_BUFFER_SPACE;
		buckets_added += Four.buckets_amount;
		Five.buckets_amount = TOTAL_BUCKETS_AMOUNT - buckets_added;
		buckets_added += Five.buckets_amount;

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

		// Maybe additional "swap space" will be needed

		size_t vertices_in_zero_zone = Zero.max_vertices_per_bucket * Zero.buckets_amount;
		Zero.start = buffer;
		Zero.end = buffer + vertices_in_zero_zone;
		Zero.start_offset = 0;
		Zero.end_offset = vertices_in_zero_zone;

		size_t vertices_in_first_zone = Two.max_vertices_per_bucket * Two.buckets_amount;
		One.start = Zero.end;
		One.end = One.start + vertices_in_first_zone;
		One.start_offset = Zero.end_offset;
		One.end_offset = One.start_offset + vertices_in_first_zone;

		size_t vertices_in_second_zone = Three.max_vertices_per_bucket * Three.buckets_amount;
		Two.start = One.end;
		Two.end = Two.start + vertices_in_first_zone;
		Two.start_offset = One.end_offset;
		Two.end_offset = One.start_offset + vertices_in_first_zone;

		size_t vertices_in_third_zone = Four.max_vertices_per_bucket * Four.buckets_amount;
		Three.start = Two.end;
		Three.end = Three.start + vertices_in_first_zone;
		Three.start_offset = Two.end_offset;
		Three.end_offset = Three.start_offset + vertices_in_first_zone;

		size_t vertices_in_fourth_zone = Five.max_vertices_per_bucket * Five.buckets_amount;
		Four.start = Three.end;
		Four.end = Four.start + vertices_in_first_zone;
		Four.start_offset = Three.end_offset;
		Four.end_offset = Four.start_offset + vertices_in_first_zone;

		Five.start = Four.end;
		Five.end = buffer + total_vertex_amount;
		Five.start_offset = Four.end_offset;
		Five.end_offset = total_vertex_amount;
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
		Zone chooseZone(size_t vertices_amount);
		std::pair<size_t, size_t> getBucketIdFromDAIC(DAIC daic);

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