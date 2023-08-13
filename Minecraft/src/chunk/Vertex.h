#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "VertexCompresser.h"

struct Vertex
{
	Vertex(glm::ivec3 xyz, glm::ivec3 uvw, int s)
	{
		_xyzs = VertexCompresser::compress_xyzs(xyz, s);
		_uvw = VertexCompresser::compress_uvw(uvw);
	}
	GLuint _xyzs;
	GLuint _uvw;
};

//struct Vertex
//{
//	Vertex(glm::vec3 p, glm::vec3 t, float s) {
//		position[0] = p.x;
//		position[1] = p.y;
//		position[2] = p.z;
//		texture[0] = t.x;
//		texture[1] = t.y;
//		texture[2] = t.z;
//		shading = s;
//	}
//
//	float position[3];
//	float texture[3];
//	float shading;
//};