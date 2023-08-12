#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
//struct Vertex
//{
//	GLuint xyzs;
//	GLuint uvw;
//	GLuint lod_scale; // TODO: pass lod_scale once per chunk
//	GLuint chunk_world_pos[3]; // TODO: pass chunk_world_pos once per chunk, not once per vertex
//};

struct Vertex
{
	Vertex(glm::vec3 p, glm::vec3 t, float s) {
		position[0] = p.x;
		position[1] = p.y;
		position[2] = p.z;
		texture[0] = t.x;
		texture[1] = t.y;
		texture[2] = t.z;
		shading = s;
	}

	float position[3];
	float texture[3];
	float shading;
};