#pragma once
#include <vector>
#include <glad/glad.h>
#include <optick.h>
#include "../chunk/MeshData.h"


class ChunkMeshLoader 
{
public:
	ChunkMeshLoader();
	~ChunkMeshLoader() = default;
	void loadMesh(const MeshData& mesh);
	void loadPackedMesh(const PackedMeshData& packed_mesh);
	void bindVAO() const;
	void unbindVAO() const;

	void cleanPackedBuffers() const;
	
private:
	GLuint m_vao;
	GLuint m_vertex_vbo, m_texture_vbo, m_shading_vbo;
	GLuint m_xyzs_vbo, m_uvw_vbo;

	void createVAO();
	void storeDataInVAO(const MeshData& mesh);
	void storePackedDataInVAO(const PackedMeshData& packed_mesh);
	void cleanBuffers() const;
};