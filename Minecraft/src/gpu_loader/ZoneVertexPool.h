#pragma once
#include <glad/glad.h>
#include "../chunk/Vertex.h"
#include "../renderer/ChunkRendererSettings.h"
#include "../chunk/ChunkSize.h"
#include "../chunk/Chunk.h"
#include "Zone.h"
#include <vector>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <glm/glm.hpp>
#include "../loguru.hpp"

#include "optick.h"

namespace VertexPool {
	constexpr size_t MAX_BLOCKS_IN_CHUNK = CHUNK_SIZE * CHUNK_SIZE;
	constexpr size_t FACES_IN_BLOCK = 6;

	using namespace ChunkRendererSettings;
	//constexpr size_t MIN_BUCKETS_AMOUNT = 39996 * MAX_RENDERED_CHUNKS_IN_Y_AXIS;
	constexpr size_t TOTAL_CHUNKS = MAX_RENDERED_CHUNKS_IN_XZ_AXIS * MAX_RENDERED_CHUNKS_IN_XZ_AXIS * MAX_RENDERED_CHUNKS_IN_Y_AXIS;
	constexpr size_t TOTAL_BUCKETS_AMOUNT = TOTAL_CHUNKS; //std::max(TOTAL_CHUNKS, MIN_BUCKETS_AMOUNT);

	static const size_t MAX_DAIC_AMOUNT = TOTAL_BUCKETS_AMOUNT;

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
		DAIC(unsigned int c, unsigned int ic, unsigned int f, unsigned int bv)
		{
			count = c;
			instance_count = ic;
			first = f;
			base_vertex = bv;
		}

		unsigned int count;
		unsigned int instance_count;
		unsigned int first;
		unsigned int base_vertex;
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
		size_t added_faces;
	};

	struct ChunkAllocData
	{
		glm::ivec3 _chunk_pos;
		size_t _added_faces_amount;
		LevelOfDetail::LevelOfDetail _lod;
		std::vector<Vertex> _mesh;
		std::vector<Face> _mesh_faces;
		glm::ivec3 _chunk_world_pos;
		bool _ready = false;

		ChunkAllocData()
		{
			_ready = false;
		}

		ChunkAllocData(glm::ivec3 chunk_pos, size_t added_faces_amount, LevelOfDetail::LevelOfDetail lod, std::vector<Vertex> mesh, std::vector<Face> mesh_faces, glm::ivec3 chunk_world_pos)
			: _chunk_pos{chunk_pos},
			_added_faces_amount{added_faces_amount},
			_lod{lod},
			_mesh{mesh},
			_mesh_faces{mesh_faces},
			_chunk_world_pos{chunk_world_pos}
		{}
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
		void allocate(ChunkAllocData&& alloc_data);
		void free(glm::ivec3 chunk_pos);
		void createChunkInfoBuffer();
		void createChunkLodBuffer();

	private:
		void initBuckets();
		void initZones(Vertex* buffer);
		void createMeshBuffer();
		void updateMeshBuffer(std::vector<Vertex>& mesh, int buffer_offset);
		void createFaceStreamBuffer();
		void updateFaceStreamBuffer(std::vector<Face>& mesh, int face_offset);
		void formatVBO();
		void waitBuffer(GLsync& sync);
		void lockBuffer(GLsync& sync);
		void fastErase(glm::ivec3 chunk_pos);

		MeshBucket* getFirstFreeBucket(int zone_id);
		Zone chooseZone(unsigned int lod_level);
		std::pair<size_t, size_t> getBucketIdFromDAIC(DAIC daic);
		size_t calculateBucketAmountInZones();
		size_t calculateTotalVertexAmount();
		Zone calculateZoneFromDaicStartOffset(DAIC daic);

		GLuint m_vao;
		GLuint m_vbo;
		GLuint m_daicbo;
		GLuint m_chunk_info_ssbo;
		GLuint m_chunks_lod_ssbo;
		GLuint m_face_stream_ssbo;

		Vertex* m_mesh_persistent_buffer;
		size_t m_persistent_buffer_vertices_amount;
		GLsync m_sync;

		Face* m_face_stream_buffer;
		size_t m_mesh_faces_amount;
		GLsync m_face_buffer_sync;


		ChunkMetadata m_chunk_metadata;
		std::unordered_map<glm::ivec3, std::pair<size_t, size_t>> m_chunk_pos_to_bucket_id;
		std::unordered_map<std::pair<size_t, size_t>, size_t, pair_hash> m_bucket_id_to_daic_id;
		VertexPoolStats m_stats;

		std::vector<std::vector<MeshBucket>> m_chunk_buckets;
	};

}