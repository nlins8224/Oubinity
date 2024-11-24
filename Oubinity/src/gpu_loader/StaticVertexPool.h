//#pragma once
//#include <glad/glad.h>
//#include "../chunk/Vertex.h"
//#include "../renderer/ChunkRendererSettings.h"
//#include "../chunk/ChunkSize.h"
//#include "../chunk/Chunk.h"
//#include <vector>
//#include <iostream>
//#include <unordered_map>
//#include <algorithm>
//#include <glm/glm.hpp>
//#include "../loguru.hpp"
//
//#include "optick.h"
//
///*
// This VertexPool is a simple one, with one fixed bucket size.
// It is used mainly for debug purposes, to compare results with other pools
//*/
// namespace DebugVertexPool {
//	constexpr int CHUNKS_AMOUNT =
//ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS *
//ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS *
//ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS; 	constexpr int
//BACKUP_BUFFER = 2000; 	constexpr int MAX_BLOCKS_IN_CHUNK = CHUNK_SIZE *
//CHUNK_SIZE; 	constexpr int FACES_IN_BLOCK = 6; 	constexpr int VERTICES_IN_FACE =
//6;
//
//	constexpr int MAX_ADDED_VERTICES = MAX_BLOCKS_IN_CHUNK * FACES_IN_BLOCK
//* VERTICES_IN_FACE;
//
//	constexpr size_t MAX_VERTICES_IN_BUCKET = MAX_ADDED_VERTICES;
//	constexpr size_t MAX_DAIC_AMOUNT = CHUNKS_AMOUNT + BACKUP_BUFFER;
//
// struct ChunkInfo
//{
//	glm::vec4 chunk_pos[MAX_DAIC_AMOUNT];
//};
//
// struct ChunksLod
//{
//	GLuint chunks_lod[MAX_DAIC_AMOUNT];
//};
//
// struct MeshBucket
//{
//	bool _is_free{ true };
//	Vertex* _start_ptr;
//	unsigned int _start_offset;
//	size_t _id;
//};
//
// struct DAIC
//{
//	DAIC(unsigned int c, unsigned int ic, unsigned int f, unsigned int bi)
//	{
//		count = c;
//		instance_count = ic;
//		first = f;
//		base_instance = bi;
//	}
//
//	unsigned int count;
//	unsigned int instance_count;
//	unsigned int first;
//	unsigned int base_instance;
//};
//
//// All struct elements in ChunkMetadata share the same ID
// struct ChunkMetadata
//{
//	ChunkInfo active_chunk_info;
//	ChunksLod active_chunks_lod;
//	std::vector<DAIC> active_daics;
//};
//
// struct VertexPoolStats
//{
//	size_t max_vertices_occurred;
//	size_t min_vertices_occurred;
//};
//
// class StaticVertexPool
//{
// public:
//	StaticVertexPool();
//	virtual ~StaticVertexPool();
//	void draw();
//	void allocate(Chunk& chunk);
//	void free(glm::ivec3 chunk_pos);
//
//	void createChunkInfoBuffer();
//	void createChunkLodBuffer();
// private:
//	void initBuckets();
//	void createMeshBuffer();
//	void updateMeshBuffer(std::vector<Vertex>& mesh, int buffer_offset);
//	void formatVBO();
//	void waitBuffer();
//	void lockBuffer();
//	void fastErase(glm::ivec3 chunk_pos);
//	MeshBucket* getFirstFreeBucket();
//	size_t getBucketIdFromStartOffset(size_t offset);
//
//	GLuint m_vao;
//	GLuint m_vbo;
//	GLuint m_daicbo;
//	GLuint m_chunk_info_ssbo;
//	GLuint m_chunks_lod_ssbo;
//	size_t m_vertices_amount;
//
//	Vertex* m_mesh_persistent_buffer;
//	MeshBucket chunk_buckets[DebugVertexPool::MAX_DAIC_AMOUNT];
//	GLsync m_sync;
//
//	ChunkMetadata m_chunk_metadata;
//	std::unordered_map<glm::ivec3, size_t> m_chunk_pos_to_bucket_id;
//	std::unordered_map<size_t, size_t> m_bucket_id_to_daic_id;
//	VertexPoolStats m_stats;
//};
//
//}