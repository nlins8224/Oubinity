#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>

class VertexCompresser
{
public:
	/*
	Passing integer values is required here.
	Compressing Vertex is possible because each chunk starts at integer coords, each block is 1x1x1 size and each face has {0.0, 1.0} coords.
	*/
	//static Vertex compress(glm::ivec3 xyz_coords, glm::ivec3 uvw_coords, int shading_value);
	static GLuint compress_xyzs(glm::ivec3 xyz_coords, int shading_value);
	static GLuint compress_uvw(glm::ivec3 uvw_coords);
	static GLuint compress_xyzs(GLubyte x, GLubyte y, GLubyte z, GLubyte shading_value);
	static GLuint compress_uvw(GLubyte u, GLubyte v, GLubyte w);
};