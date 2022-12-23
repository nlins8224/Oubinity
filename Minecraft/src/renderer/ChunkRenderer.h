#pragma once
#include "Renderer.h"
#include "../Camera.h"
#include "../shader/ChunkShader.h"
#include "../chunk/ChunksMap.h"
#include "../chunk/Chunk.h"

class ChunkRenderer : public Renderer
{
public:
	ChunkRenderer(Shader shader);
	ChunkRenderer() = delete;
	~ChunkRenderer();
	void render(Camera& camera) override;
	void setChunks(ChunksMap* chunks);
private:
	void draw(const Mesh& mesh) const override;
	void renderChunk(Camera& camera, Chunk& chunk) const;
	ChunksMap* m_chunks_map;
};