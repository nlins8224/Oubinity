#pragma once
#include <atomic>
#include <map>
#include <queue>
#include <vector>
#include "../Camera.h"
#include "../chunk/Chunk.h"
#include "../chunk/ChunkSize.h"
#include "../chunk/ChunksMap.h"
#include "../frustum/AABox.h"
#include "../gpu_loader/ZoneVertexPool.h"
#include "../shader/ChunkShader.h"
#include "../terrain_generation/TerrainGenerator.h"
#include "ChunkBorder.h"
#include "ChunkRendererSettings.h"
#include "ChunkSlidingWindow.h"
#include "Renderer.h"

#include "../loguru.hpp"
#include "../third_party/BS_thread_pool.hpp"

class ChunkRenderer : public Renderer {
 public:
  ChunkRenderer() = delete;
  ChunkRenderer(TerrainGenerator& terrain_generator, Shader shader,
                Camera& camera, GLuint texture_array);
  void render(Camera& camera) override;

  void traverseScene();
  void doIterate(int camera_chunk_pos_x, int camera_chunk_pos_z);
  void updateBufferIfNeedsUpdate();
  void runTraverseSceneInDetachedThread();
  void drawChunksSceneMesh();
  void traverseSceneLoop();
  Chunk* getChunkByWorldPos(glm::ivec3 world_block_pos);
  block_id getBlockIdByWorldPos(glm::ivec3 world_block_pos);
  bool isBlockPresentByWorldPos(glm::ivec3 world_block_pos);
  void updateBlockByWorldPos(glm::ivec3 world_block_pos, block_id type);

 private:
  void initChunks();
  bool createChunksInRenderDistance();  // called when scene was already traversed
  bool createChunkIfNotPresent(glm::ivec3 chunk_pos);
  void createChunk(glm::ivec3 chunk_pos);
  HeightMap generateHeightmap(glm::ivec3 chunk_pos,
                              LevelOfDetail::LevelOfDetail lod);
  bool populateChunksNeighbors();
  bool populateChunkNeighbor(glm::ivec3 chunk_pos);
  bool generateChunksTerrain();
  bool generateChunkTerrain(glm::ivec3 chunk_pos);
  bool decorateChunkIfPresent(glm::ivec3 chunk_pos);
  bool decorateChunks();
  bool meshChunks();
  bool meshChunk(glm::ivec3 chunk_pos);
  bool deleteOutOfRenderDistanceChunks();  // called when scene was already traversed
  bool deleteChunkIfPresent(glm::ivec3 chunk_pos);
  void deleteChunk(glm::ivec3 chunk_pos);
  bool checkIfChunkLodNeedsUpdate(glm::ivec3 chunk_pos);
  void iterateOverChunkBorderAndCreate(WindowMovementDirection move_dir);
  void iterateOverChunkBorderAndDelete(WindowMovementDirection move_dir);
  void iterateOverChunkBorderAndUpdateLod(ChunkBorder chunk_border);
  bool isChunkOutOfBorder(glm::ivec3 chunk_pos, ChunkBorder chunk_border);

  void refreshChunk(glm::ivec3 chunk_pos);

  void allocateChunks();
  void allocateChunk(glm::ivec3 chunk_pos);
  void freeChunks();
  void freeChunk(glm::ivec3 chunk_pos);

  void updateChunkPipeline();

  bool checkCameraPosChanged();

#if SETTING_USE_PRELOADED_HEIGHTMAP
  bool generatePreloadedChunkUndergroundLayer(glm::ivec3 chunk_pos);
#endif

  Camera& m_camera;
  glm::ivec3 m_camera_last_chunk_pos;
  GLuint m_texture_array;

  // Meshing is done on render thread, but allocate and free are
  // done on main thread, because of OpenGL context requirements
  std::atomic<bool> m_buffer_needs_update;

  ChunkSlidingWindow
      m_chunks_by_coord;  // shared between render and main threads
  std::vector<glm::ivec3> m_border_chunks;    // used only on render thread
  std::queue<glm::ivec3> m_chunks_to_create;  // used only on render thread

  std::queue<glm::ivec3>
      m_chunks_to_populate_neighbors;  // used only on render thread
  std::queue<glm::ivec3>
      m_chunks_to_generate_terrain;  // used only on render thread

  std::queue<glm::ivec3> m_chunks_to_decorate;  // used only on render thread

  std::queue<glm::ivec3> m_chunks_to_mesh;
  std::queue<glm::ivec3> m_chunks_to_delete;  // used only on render thread

  std::queue<glm::ivec3>
      m_chunks_to_allocate;  // render thread writes, main thread reads
  std::queue<glm::ivec3>
      m_chunks_to_free;  // render thread writes, main thread reads

  std::queue<std::function<void()>> m_tasks;

  VertexPool::ZoneVertexPool* m_vertexpool;  // called only on main thread
  TerrainGenerator& m_terrain_generator;     // called only on render thread
};