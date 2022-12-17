#pragma once
#include "Renderer.h"
#include "../Chunk.h"
#include "../Camera.h"
#include "../Shader.h"
#include "../ChunkShader.h"

class ChunkRenderer : public Renderer
{
public:
	ChunkRenderer(Shader shader);
	ChunkRenderer() = delete;
	~ChunkRenderer() = default;
	void render(Camera& camera) override;
	void setChunks(std::vector<Chunk> chunks);
private:
	void draw(Mesh& mesh) const override;
	void renderChunk(Camera& camera, Chunk chunk);
	std::vector<Chunk> m_chunks;
};