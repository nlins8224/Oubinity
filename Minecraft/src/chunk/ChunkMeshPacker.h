#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

struct PackedMesh
{
	GLuint packed_xyzs;
	GLuint packed_uvw;
};

class ChunkMeshPacker
{
public:
	/*
	Passing integer values is required here. 
	Packing chunk is possible because each chunk starts at integer coords, each block is 1x1x1 size and each face has {0.0, 1.0} coords.
	*/
	static PackedMesh pack(glm::ivec3 xyz_coords, glm::ivec3 uvw_coords, int shading_value);
	static GLuint pack_xyzs(glm::ivec3 xyz_coords, int shading_value);
	static GLuint pack_uvw(glm::ivec3 uvw_coords);
	static GLuint pack_xyzs(GLubyte x, GLubyte y, GLubyte z, GLubyte shading_value);
	static GLuint pack_uvw(GLubyte u, GLubyte v, GLubyte w);
	// TODO:
	// void unpack();
	// void unpack_xyzs();
	// void unpack_uvw();
};