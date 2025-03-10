#pragma once
#include <glad/glad.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include "../Settings.h"
#include "../chunk/Chunk.h"
#include "../chunk/Vertex.h"
#include "../loguru.hpp"
#include "Zone.h"

namespace VertexPool {
using Settings::CHUNK_SIZE;
constexpr size_t MAX_BLOCKS_IN_CHUNK = CHUNK_SIZE * CHUNK_SIZE;
constexpr size_t FACES_IN_BLOCK = 6;

using Settings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS;
using Settings::MAX_RENDERED_CHUNKS_IN_Y_AXIS;
constexpr size_t TOTAL_CHUNKS =
    MAX_RENDERED_CHUNKS_IN_XZ_AXIS * MAX_RENDERED_CHUNKS_IN_XZ_AXIS * MAX_RENDERED_CHUNKS_IN_Y_AXIS;
constexpr size_t TOTAL_BUCKETS_AMOUNT = TOTAL_CHUNKS;
constexpr uint16_t BUFFER_NEEDS_UPDATE = Settings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS / 4;
constexpr size_t ZONE_INITIAL_BUCKET_AMOUNT_MARGIN = 2000;

static const size_t MAX_DAIC_AMOUNT = TOTAL_BUCKETS_AMOUNT;

struct ChunkInfo {
  glm::vec4 chunk_pos[MAX_DAIC_AMOUNT];
};

struct ChunksLod {
  GLuint chunks_lod[MAX_DAIC_AMOUNT];
};

struct MeshBucket {
  bool _is_free{true};
  unsigned int _start_offset;
  size_t _id;
};

struct DAIC {
  DAIC(unsigned int c, unsigned int ic, unsigned int f, unsigned int bv) {
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
struct ChunkMetadata {
  ChunkInfo active_chunk_info;
  ChunksLod active_chunks_lod;
  std::vector<DAIC> active_daics;
};

struct VertexPoolStats {
  std::array<size_t, 6> max_vertices_occurred;
  std::array<size_t, 6> min_vertices_occurred;
  std::array<size_t, 6> chunks_in_buckets;
  size_t added_faces;
};

struct ChunkAllocData {
  glm::ivec3 _chunk_pos;
  size_t _added_faces_amount;
  LevelOfDetail::LevelOfDetail _lod;
  std::vector<Vertex> _mesh;
  std::vector<Face> _mesh_faces;
  glm::ivec3 _chunk_world_pos;
  bool _ready = false;

  ChunkAllocData() { _ready = false; }

  ChunkAllocData(glm::ivec3 chunk_pos, size_t added_faces_amount,
                 LevelOfDetail::LevelOfDetail lod, std::vector<Vertex> mesh,
                 std::vector<Face> mesh_faces, glm::ivec3 chunk_world_pos)
      : _chunk_pos{chunk_pos},
        _added_faces_amount{added_faces_amount},
        _lod{lod},
        _mesh{mesh},
        _mesh_faces{mesh_faces},
        _chunk_world_pos{chunk_world_pos} {}
};

struct pair_hash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2>& pair) const {
    return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
  }
};

class ZoneVertexPool {
 public:
  ZoneVertexPool();
  virtual ~ZoneVertexPool();
  void push_allocate(ChunkAllocData&& alloc_data, bool fast_path);
  void push_free(glm::ivec3 chunk_pos, bool fast_path);
  void push_update_lod(ChunkAllocData&& alloc_data);
  void draw();
  void commitUpdate();

 private:
  void initBuckets();
  void initZones(Vertex* buffer);
  void createMeshBuffer();
  void createDAICBuffer();
  void updateMeshBuffer(std::vector<Vertex>& mesh, int buffer_offset);
  void updateMeshBufferDAIC();
  void createFaceStreamBuffer();
  void updateFaceStreamBuffer(std::vector<Face>& mesh, int face_offset);
  void createChunkInfoBuffer();
  void createChunkLodBuffer();
  void updateChunkInfoBuffer();
  void updateChunkLodBuffer();
  void formatVBO();
  void waitBuffer(GLsync& sync);
  void lockBuffer(GLsync& sync);
  void fastErase(glm::ivec3 chunk_pos);
  void allocate(ChunkAllocData&& alloc_data);
  void free(glm::ivec3 chunk_pos);
  bool wasChunkAllocated(glm::ivec3 chunk_pos);

  MeshBucket* getFirstFreeBucket(int zone_id);
  MeshBucket* getBucketHoldingChunk(glm::ivec3 chunk_pos);
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
  std::unordered_map<glm::ivec3, std::pair<size_t, size_t>>
      m_chunk_pos_to_bucket_id;
  std::unordered_map<std::pair<size_t, size_t>, size_t, pair_hash>
      m_bucket_id_to_daic_id;
  VertexPoolStats m_stats;

  std::vector<std::vector<MeshBucket>> m_chunk_buckets;
  int m_buffer_needs_update_count;

  std::queue<ChunkAllocData> m_pending_allocations;
  std::queue<glm::ivec3> m_pending_frees;
  std::queue<ChunkAllocData> m_pending_lod_updates;
};

}  // namespace VertexPool