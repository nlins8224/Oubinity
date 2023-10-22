#pragma once
#include <glad/glad.h>
#include "../chunk/Vertex.h"
#include "../renderer/ChunkRendererSettings.h"
#include "../chunk/ChunkSize.h"
#include "../chunk/Chunk.h"
#include "DAIC.h"
#include <vector>
#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>
#include "../loguru.hpp"

#include "optick.h"

constexpr int CHUNKS_AMOUNT = ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS * ChunkRendererSettings::MAX_RENDERED_CHUNKS_IN_Y_AXIS;
constexpr int BACKUP_BUFFER = 2000;
constexpr int MAX_BLOCKS_IN_CHUNK = CHUNK_SIZE * CHUNK_SIZE;
constexpr int FACES_IN_BLOCK = 6;
constexpr int VERTICES_IN_FACE = 6;

constexpr int MAX_DAIC_AMOUNT = 20000;
constexpr int MAX_ADDED_VERTICES = MAX_BLOCKS_IN_CHUNK * FACES_IN_BLOCK * VERTICES_IN_FACE;

constexpr size_t MAX_VERTICES_IN_BUCKET = MAX_ADDED_VERTICES;
constexpr size_t BUCKETS_AMOUNT = CHUNKS_AMOUNT + BACKUP_BUFFER;

struct ChunkInfo
{
	glm::vec4 chunk_pos[BUCKETS_AMOUNT];
};

struct ChunksLod
{
	GLuint chunks_lod[BUCKETS_AMOUNT];
};

struct MeshBucket
{
	bool _is_free{ true };
	Vertex* _start_ptr;
	unsigned int _start_offset;
	size_t _occupied_space;
	size_t _id;
};

struct VertexPoolStats
{
	int max_vertices_occurred;
};

class VertexPool
{
public:
	VertexPool();
	virtual ~VertexPool();
	void draw();
	void allocate(Chunk& chunk);
	void free(size_t block_id, glm::ivec3 chunk_pos); // TODO: should take Chunk&
	void createMeshBuffer();
	void updateMeshBuffer(std::vector<Vertex>& mesh, int buffer_offset);
	void createChunkInfoBuffer();
	void createChunkLodBuffer();

private:
	void initBuckets();
	void formatVBO();
	void waitBuffer();
	void lockBuffer();
	void collectChunkShaderMetadata();
	MeshBucket* getFirstFreeBucket();

	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_daicbo;
	GLuint m_chunk_info_ssbo;
	GLuint m_chunks_lod_ssbo;
	size_t m_vertices_amount;

	MeshBucket m_chunk_buckets[BUCKETS_AMOUNT];
	Vertex* m_mesh_persistent_buffer;
	GLsync m_sync;

	std::vector<DAIC> m_active_daics;
	ChunkInfo m_active_chunks_info;
	ChunksLod m_active_chunks_lod;

	VertexPoolStats m_stats;
};