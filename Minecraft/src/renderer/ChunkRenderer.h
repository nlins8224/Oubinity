#pragma once
#include "Renderer.h"
#include "../Chunk.h"
#include "../Camera.h"

class ChunkRenderer : public Renderer
{
public:
	void render(Camera& camera) override;
	void add(Chunk* chunk);
private:
	void draw(Mesh& mesh) const override;
	void renderChunk(Camera& camera, Chunk* chunk);
	//TODO: Better to have smart pointers here?
	std::vector<Chunk*> m_chunks;
	Shader m_shader;
};