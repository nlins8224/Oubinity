#version 460 core
layout (location = 0) in uint vertex;

out vec3 tex_coords;
out float shading_values;
out float visibility;
out float height;
out vec4 debug_color;

const float fog_density = 0.0004;
const float fog_gradient = 1.1;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (std430, binding = 0) readonly buffer ChunkInfo
{
	vec4 chunk_pos[];
} chunkInfo;

layout (std430, binding = 1) readonly buffer Lod
{
	uint block_size[];
} lod;

layout (std430, binding = 2) readonly buffer FaceStream
{
	uint face[];
} faceStream;

/*
winding order is counter-clockwise, but remember that it's relative to
viewer's point of view, e.g. back face winding is "reversed".

 v0 - (0, 0, 1)
 v1 - (1, 0, 1)
 v2 - (0, 1, 1)
 v3 - (1, 1, 1)
 v4 - (0, 0, 0)
 v5 - (1, 0, 0)
 v6 - (0, 1, 0)
 v7 - (1, 1, 0)
 
      ^ Y
      |
      |
      v6------v7
     /        / |
    /        /  |
   v2------v3   |
   |        |   |
   |  v4----|--v5  --> X
   | /      |  /
   |/       | /
   v0-------v1
  /
 /
Z 

 
*/

vec2 tex[4] = vec2[4](
    vec2(0.0f, 0.0f), // v0 
	vec2(1.0f, 0.0f), // v1
    vec2(1.0f, 1.0f), // v3 
    vec2(0.0f, 1.0f)  // v2
);

vec3 back_face[4] = vec3[4](
	vec3(0.0f, 0.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f),
	vec3(1.0f, 1.0f, 0.0f),
	vec3(1.0f, 0.0f, 0.0f)
);

vec3 front_face[4] = vec3[4](
	vec3(0.0f, 0.0f, 1.0f),
	vec3(0.0f, 1.0f, 1.0f),
	vec3(1.0f, 1.0f, 1.0f),
	vec3(1.0f, 0.0f, 1.0f) 
);

vec3 left_face[4] = vec3[4](
	vec3(0.0f, 0.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, 1.0f, 1.0f),
	vec3(0.0f, 0.0f, 1.0f) 
);

vec3 right_face[4] = vec3[4](
	vec3(1.0f, 0.0f, 0.0f),
	vec3(1.0f, 1.0f, 0.0f),
	vec3(1.0f, 1.0f, 1.0f),
	vec3(1.0f, 0.0f, 1.0f) 
);

vec3 top_face[4] = vec3[4](
	vec3(0.0f, 1.0f, 0.0f),
	vec3(1.0f, 1.0f, 0.0f),
	vec3(1.0f, 1.0f, 1.0f),
	vec3(0.0f, 1.0f, 1.0f) 
);

vec3 bottom_face[4] = vec3[4](
	vec3(0.0f, 0.0f, 0.0f),
	vec3(1.0f, 0.0f, 0.0f),
	vec3(1.0f, 0.0f, 1.0f),
	vec3(0.0f, 0.0f, 1.0f)  
);

float shading_table[6] = float[6](0.8f, 0.8f, 0.6f, 0.6f, 1.0f, 0.4f);
float ambient_occlusion_values[4] = float[4](0.25f, 0.5f, 0.75f, 1.0f);

uint back_face_indices[6]   = uint[6](3, 0, 1, 3, 1, 2);
uint front_face_indices[6]  = uint[6](3, 1, 0, 3, 2, 1);
uint left_face_indices[6]   = uint[6](3, 1, 0, 3, 2, 1);
uint right_face_indices[6]  = uint[6](3, 0, 1, 3, 1, 2);
uint top_face_indices[6]    = uint[6](1, 0, 3, 1, 3, 2);
uint bottom_face_indices[6] = uint[6](1, 3, 0, 1, 2, 3);

void main()
{
	// VERTICES_PER_FACE and face_idx has to be uint, otherwise
	// expression result is incorrect for gl_VertexID > 9e5, 
	// don't know why, AFAIK GLSL spec states that ints could be used here as well
	uint VERTICES_PER_FACE = 6;
	uint face_idx = uint(gl_VertexID) / VERTICES_PER_FACE;

	uint target_face = faceStream.face[face_idx];
	
	uint x          = target_face          & 31u; // 5 bits
	uint y          = (target_face >> 5u)  & 31u; // 5 bits
	uint z          = (target_face >> 10u) & 31u; // 5 bits
	uint texture_id = (target_face >> 15u) & 31u; // 5 bits
	uint face_id    = (target_face >> 20u) & 7u;  // 3 bits
	uint ao_v0      = (target_face >> 23u) & 3u;  // 2 bits
	uint ao_v1      = (target_face >> 25u) & 3u;  // 2 bits
	uint ao_v2      = (target_face >> 27u) & 3u;  // 2 bits
	uint ao_v3      = (target_face >> 29u) & 3u;  // 2 bits

	uint vertex_id  = gl_VertexID % 6;

	vec3 vertex_pos = vec3(x, y, z);

	// prevent anisotropy
	if (ao_v1 + ao_v3 > ao_v0 + ao_v2)
	{	
		back_face_indices   = uint[6](3,0,1,3,1,2);
		front_face_indices  = uint[6](3,1,0,3,2,1);
		left_face_indices   = uint[6](3,1,0,3,2,1);
		right_face_indices  = uint[6](3,0,1,3,1,2);
		top_face_indices    = uint[6](1,0,3,1,3,2);
	    bottom_face_indices = uint[6](1,3,0,1,2,3);
	}


	uint index = 0;
	uint ao_corners[4] = uint[4](ao_v0, ao_v1, ao_v2, ao_v3);
	float ao_val = 1.0f;
	switch(face_id)
	{
		case 0u: // back 
			index = back_face_indices[vertex_id];
			vertex_pos += back_face[index];
			ao_val = ambient_occlusion_values[ao_corners[index]];
			break;
		case 1u: // front 
			index = front_face_indices[vertex_id];
			vertex_pos += front_face[index];
			ao_val = ambient_occlusion_values[ao_corners[index]];
			break;
		case 2u: // left 
			index = left_face_indices[vertex_id];
			vertex_pos += left_face[index];
			ao_val = ambient_occlusion_values[ao_corners[index]];
			break;
		case 3u: // right
			index = right_face_indices[vertex_id];
			vertex_pos += right_face[index];
			ao_val = ambient_occlusion_values[ao_corners[index]];
			break;
		case 4u: // top
			index = top_face_indices[vertex_id];
			vertex_pos += top_face[index];
			ao_val = ambient_occlusion_values[ao_corners[index]];
			break;
		case 5u: // bottom
			index = bottom_face_indices[vertex_id];
			vertex_pos += bottom_face[index];
			ao_val = ambient_occlusion_values[ao_corners[index]];
			break;
	}
	tex_coords = vec3(tex[index], texture_id);
	shading_values = shading_table[face_id] * ao_val;

	vertex_pos *= lod.block_size[gl_DrawID];
	/*
	Surface height is determined first, block is placed after calculations.
	This means that larger blocks will not align with smaller blocks. Larger block bottom face
	will be neighbour of smaller block top face.
	*/
	vertex_pos.y -= lod.block_size[gl_DrawID] - 1;

	vertex_pos.x += chunkInfo.chunk_pos[gl_DrawID].x;
	vertex_pos.y += chunkInfo.chunk_pos[gl_DrawID].y;
	vertex_pos.z += chunkInfo.chunk_pos[gl_DrawID].z;

	gl_Position = projection * view * model * vec4(vertex_pos, 1.0);

	// Calculate fog
	vec4 pos_to_cam = view * vec4(vertex_pos, 1.0);
	float fog_distance = length(pos_to_cam.xyz);
	visibility = exp(-pow((fog_distance * fog_density), fog_gradient));
	visibility = clamp(visibility, 0.0, 1.0);
    height = vertex_pos.y;
}