#pragma once
#include "Renderer.h"
#include "../Chunk.h"
#include "../Camera.h"
#include "../Shader.h"
#include "../ChunkShader.h"
#include "../ChunksMap.h"

class ChunkRenderer : public Renderer
{
public:
	ChunkRenderer(Shader shader);
	ChunkRenderer() = delete;
	~ChunkRenderer() = default;
	void render(Camera& camera) override;
	void setChunks(ChunksMap& chunks);
private:
	void draw(Mesh& mesh) const override;
	void renderChunk(Camera& camera, Chunk chunk);
	std::unique_ptr<ChunksMap> m_chunks_map;
};