#pragma once
#include "Renderer.h"
#include "../Camera.h"
#include "../shader/ChunkShader.h"
#include "../chunk/ChunksMap.h"
#include "../chunk/ChunkSize.h"
#include "../chunk/Chunk.h"
#include "../frustum/AABox.h"

class ChunkRenderer : public Renderer
{
public:
	ChunkRenderer() = delete;
	ChunkRenderer(Shader shader, ChunksMap& chunks_map);
	~ChunkRenderer();
	void render(Camera& camera) override;
	void setChunks(ChunksMap* chunks);
	void processChunksMesh();
private:
	void draw(const Mesh& mesh) const override;
	void processChunkMesh(Chunk& chunk) const;
	void loadChunkMesh(Chunk& chunk) const;
	bool isInFrustum(Camera& camera, Chunk& chunk) const;
	void renderChunk(Camera& camera, Chunk& chunk) const;
	ChunksMap& m_chunks_map;
};