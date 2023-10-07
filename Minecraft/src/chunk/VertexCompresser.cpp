#include "VertexCompresser.h"

//Vertex VertexCompresser::compress(glm::ivec3 xyz_coords, glm::ivec3 uvw_coords, int shading_value)
//{
//	GLuint packed_xyzs = compress_xyzs(xyz_coords, shading_value);
//	GLuint packed_uvw = compress_uvw(uvw_coords);
//	
//	return {packed_xyzs, packed_uvw};
//}

GLuint VertexCompresser::compress_xyzs(glm::ivec3 xyz_coords, int shading_value)
{
	return xyz_coords.x
		| xyz_coords.y << 6
		| xyz_coords.z << 12
		| shading_value << 18;
}

GLuint VertexCompresser::compress_uvw(glm::ivec3 uvw_coords)
{
	return uvw_coords.x
		| uvw_coords.y << 6
		| uvw_coords.z << 12;
}

GLuint VertexCompresser::compress_xyzs(GLubyte x, GLubyte y, GLubyte z, GLubyte shading_value)
{
	return x | y << 6 | z << 12 | shading_value << 18;
}

GLuint VertexCompresser::compress_uvw(GLubyte u, GLubyte v, GLubyte w)
{
	return u | v << 6 | w << 12;
}
