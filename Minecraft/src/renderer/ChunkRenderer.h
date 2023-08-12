#pragma once
#include <map>
#include <vector>
#include <queue>
#include "Renderer.h"
#include "../terrain_generation/TerrainGenerator.h"
#include "../gpu_loader/VertexPool.h"
#include "../Camera.h"
#include "../shader/ChunkShader.h"
#include "../chunk/ChunksMap.h"
#include "../chunk/ChunkSize.h"
#include "../chunk/Chunk.h"
#include "../frustum/AABox.h"

#include "optick.h"

namespace ChunkRendererSettings
{
	constexpr int MAX_RENDERED_CHUNKS_IN_XZ_AXIS = 32;
	constexpr int MAX_RENDERED_CHUNKS_IN_Y_AXIS = 1;
}

struct ChunkMessage
{
	glm::ivec3 chunk_pos;
	enum class message
	{
		CREATE,
		DELETE
	};
};

class ChunkRenderer : public Renderer
{
public:
	ChunkRenderer() = delete;
	ChunkRenderer(Shader shader, Camera& camera, GLuint texture_array);
	void render(Camera& camera) override;

	void traverseScene();
	void createInRenderDistanceAndDestroyOutOfRenderDistanceChunks();
	void drawChunksSceneMesh();
private:
	void createInRenderDistanceChunks(); // called when scene was already traversed
	void destroyOutOfRenderDistanceChunks(); // called when scene was already traversed
	void createChunkIfNotPresent(glm::ivec3 chunk_pos);
	void createChunk(glm::ivec3 chunk_pos);
	void deleteChunk(glm::ivec3 chunk_pos);
	void processChunks();
	void processChunk(Chunk& chunk);

	Camera& m_camera;
	GLuint m_texture_array;

	std::unordered_map<glm::ivec3, Chunk> m_chunks_by_coord;
	std::vector<Chunk> m_all_chunks;
	std::queue<glm::ivec3> m_chunks_to_create;
	std::queue<glm::ivec3> m_chunks_to_delete;

	std::vector<Vertex> m_world_mesh;
	std::vector<DAIC> m_world_mesh_daic;
	VertexPool* m_vertexpool;
	TerrainGenerator* m_terrain_generator;

	unsigned int m_total_faces_added;
	bool m_buffer_needs_update;
};