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

struct Face {
	uint one;
	uint two;
};

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
	Face face[];
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

float shading_table[6] = float[6](1.0f, 0.4f, 0.6f, 0.6f, 0.8f, 0.8f);
float ambient_occlusion_values[4] = float[4](0.25f, 0.5f, 0.75f, 1.0f);

/*
3-------2
|       |
|       |
|       |
0-------1
*/
uint quad_index_lookup[6] = uint[6](1, 2, 3, 0, 1, 3);

uint w_dir_lookup[6] = uint[6](0, 0, 1, 1, 0, 0); // X, X, Y, Y, X, X
uint h_dir_lookup[6] = uint[6](2, 2, 2, 2, 1, 1); // Z, Z, Z, Z, Y, Y

uint w_mod_lookup[4] = uint[4](0, 0, 1, 1); // F, F, T, T
uint h_mod_lookup[4] = uint[4](0, 1, 1, 0); // F, T, T, F

void main()
{
	// VERTICES_PER_FACE and face_stream_idx has to be uint, otherwise
	// expression result is incorrect for gl_VertexID > 9e5
	uint VERTICES_PER_FACE = 6;
	uint face_stream_idx = uint(gl_VertexID) / VERTICES_PER_FACE;
	uint target_face_one = faceStream.face[face_stream_idx].one;
	uint target_face_two = faceStream.face[face_stream_idx].two;

	uint x          = target_face_one          & 63u; // 6 bits
	uint y          = (target_face_one >> 6u)  & 63u; // 6 bits
	uint z          = (target_face_one >> 12u) & 63u; // 6 bits
	uint w          = (target_face_one >> 18u) & 63u; // 6 bits
	uint h          = (target_face_one >> 24u) & 63u; // 6 bits
	uint face_id    = target_face_two		   & 7u;  // 3 bits
	uint texture_id = (target_face_two >> 3u)  & 31u; // 5 bits
	uint ao_v0      = (target_face_two >> 8u)  & 3u;  // 2 bits
	uint ao_v1      = (target_face_two >> 10u) & 3u;  // 2 bits
	uint ao_v2      = (target_face_two >> 12u) & 3u;  // 2 bits
	uint ao_v3      = (target_face_two >> 14u) & 3u;  // 2 bits

	if (face_id == 4 ) {
		debug_color = vec4(1.0, 0.0, 0.0, 0.0);
	} else if (face_id == 2){
		debug_color = vec4(0.0, 0.0, 1.0, 0.0);
	} else if (face_id == 0){
		debug_color = vec4(0.0, 1.0, 0.0, 0.0);
	} else if (face_id == 1) {
		debug_color = vec4(1.0, 1.0, 0.0, 0.0);
	} else if (face_id == 3) {
		debug_color = vec4(0.0, 1.0, 1.0, 0.0);
	} else if (face_id == 5) {
		debug_color = vec4(0.5, 0.5, 0.5, 0.0);
	}

	// Reverse winding order
	if (face_id == 1 || face_id == 2 || face_id == 4) {
		quad_index_lookup = uint[6](3, 1, 0, 3, 2, 1);
	}

	uint vertex_id  = gl_VertexID % 6;
	uint quad_index = quad_index_lookup[vertex_id];

	uint ao_corners[4] = uint[4](ao_v0, ao_v1, ao_v2, ao_v3);
	float ao_val = ambient_occlusion_values[ao_corners[quad_index]];

	shading_values = shading_table[face_id] * ao_val;

	vec3 vertex_pos = vec3(x, y, z);
	uint w_dir = w_dir_lookup[face_id], h_dir = h_dir_lookup[face_id];
	uint w_mod = w_mod_lookup[quad_index], h_mod = h_mod_lookup[quad_index];

	vertex_pos[w_dir] += w * w_mod;
	vertex_pos[h_dir] += h * h_mod;

	vertex_pos *= lod.block_size[gl_DrawID];
	/*
	Surface height is determined first, block is placed after calculations.
	This means that larger blocks will not align with smaller blocks. Larger block bottom face
	will be neighbour of smaller block top face.
	*/
	uint block_size = lod.block_size[gl_DrawID];
	if (block_size > 1) {
	  vertex_pos.y -= block_size * 2 + 1;
	}

	vertex_pos.x += chunkInfo.chunk_pos[gl_DrawID].x;
	vertex_pos.y += chunkInfo.chunk_pos[gl_DrawID].y;
	vertex_pos.z += chunkInfo.chunk_pos[gl_DrawID].z;

	tex[1][0] += h;
	tex[2][0] += h; 
	tex[2][1] += w;
	tex[3][1] += w;
	tex_coords = vec3(tex[quad_index], texture_id);

	gl_Position = projection * view * model * vec4(vertex_pos, 1.0);

	// Calculate fog
	vec4 pos_to_cam = view * vec4(vertex_pos, 1.0);
	float fog_distance = length(pos_to_cam.xyz);
	visibility = exp(-pow((fog_distance * fog_density), fog_gradient));
	visibility = clamp(visibility, 0.0, 1.0);
    height = vertex_pos.y;
}