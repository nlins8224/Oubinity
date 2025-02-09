#pragma once
#include <atomic>
#include <map>
#include <queue>
#include <vector>
#include "../Camera.h"
#include "../chunk/Chunk.h"
#include "../chunk/ChunksMap.h"
#include "../frustum/AABox.h"
#include "../gpu_loader/ZoneVertexPool.h"
#include "../shader/ChunkShader.h"
#include "../terrain_generation/TerrainGenerator.h"
#include "ChunkBorder.h"
#include "ChunkSlidingWindow.h"
#include "Renderer.h"
#include "../loguru.hpp"
#include "../third_party/BS_thread_pool.hpp"
#include "../third_party/concurrentqueue.h"

class ChunkRenderer : public Renderer {
 public:
  ChunkRenderer() = delete;
  ChunkRenderer(TerrainGenerator& terrain_generator, Shader shader,
                Camera& camera, GLuint texture_array);
  void render(Camera& camera) override;

  void traverseScene();
  void doIterate(int src_camera_chunk_pos_x,
                 int src_camera_chunk_pos_z, int dst_camera_chunk_pos_x,
                 int dst_camera_chunk_pos_z);
  void updateBufferIfNeedsUpdate();
  void runTraverseSceneInDetachedThread();
  void drawChunksSceneMesh();
  void traverseSceneLoop();
  bool isBlockPresentByWorldPos(glm::ivec3 world_block_pos);
  void updateBlockByWorldPos(glm::ivec3 world_block_pos, block_id type);

 private:
  void initChunks();
  bool createChunkIfNotPresent(glm::ivec3 chunk_pos);
  void createChunk(glm::ivec3 chunk_pos);
  HeightMap generateHeightmap(glm::ivec3 chunk_pos,
                              LevelOfDetail::LevelOfDetail lod);
  bool populateChunkNeighbor(glm::ivec3 chunk_pos);
  bool generateChunkTerrainIfNeeded(glm::ivec3 chunk_pos);
  bool decorateChunkIfPresent(glm::ivec3 chunk_pos);
  bool meshChunk(glm::ivec3 chunk_pos);
  bool freeChunkIfPresent(glm::ivec3 chunk_pos);
  bool markIfChunkLodNeedsUpdate(glm::ivec3 chunk_pos);
  void UpdateWorldChunkBorder(WindowMovementDirection move_dir,
                                       ChunkBorder dst_chunk_border);
  void iterateOverChunkBorderAndUpdateLod(ChunkBorder chunk_border);
  bool isChunkOutOfBorder(glm::ivec3 chunk_pos, ChunkBorder chunk_border);

  void generateChunk(glm::ivec3 chunk_pos);
  VertexPool::ChunkAllocData getAllocData(glm::ivec3 chunk_pos);
  void allocateChunk(VertexPool::ChunkAllocData alloc_data, bool fast_path);
  void freeChunk(glm::ivec3 chunk_pos, bool fast_path);

  Camera& m_camera;
  glm::ivec3 m_camera_last_chunk_pos;
  GLuint m_texture_array;
  bool m_init_stage;

  // Meshing is done on generation thread, but allocate and free are
  // done on main thread, because of OpenGL context requirements
  std::atomic<bool> m_buffer_needs_update;
  std::atomic<int> m_generation_tasks;
  std::atomic<int> m_free_tasks;
  ChunkSlidingWindow
      m_chunks_by_coord;  // shared between generation and main threads

  moodycamel::ConcurrentQueue<VertexPool::ChunkAllocData>
      m_chunks_to_allocate;  // generation thread writes, main thread reads
  moodycamel::ConcurrentQueue<glm::ivec3>
      m_chunks_to_free;  // generation thread writes, main thread reads

  BS::thread_pool m_generation_task_pool{std::min(4u, std::thread::hardware_concurrency())};

  VertexPool::ZoneVertexPool* m_vertexpool;  // called only on main thread
  TerrainGenerator& m_terrain_generator;     // called only on generation thread
};