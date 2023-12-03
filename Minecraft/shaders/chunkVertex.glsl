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

vec2 tex[4] = vec2[4](
    vec2(0.0f, 0.0f), // Bottom left
    vec2(1.0f, 1.0f), // Top right
    vec2(1.0f, 0.0f), // Bottom right
    vec2(0.0f, 1.0f)  // Top left
);

vec3 back_face[4] = vec3[4](
	vec3(0.0f, 0.0f, 0.0f), // Bottom left
	vec3(1.0f, 1.0f, 0.0f), // Top Right
	vec3(1.0f, 0.0f, 0.0f), // Top Left
	vec3(0.0f, 1.0f, 0.0f)  // Bottom Right
);

vec3 front_face[4] = vec3[4](
	vec3(0.0f, 0.0f, 1.0f), // Bottom left
	vec3(1.0f, 1.0f, 1.0f), // Top Right
	vec3(0.0f, 1.0f, 1.0f), // Top Left
	vec3(1.0f, 0.0f, 1.0f)  // Bottom Right
);

vec3 left_face[4] = vec3[4](
	vec3(0.0f, 1.0f, 1.0f), // Top right
	vec3(0.0f, 0.0f, 0.0f), // Bottom left
	vec3(0.0f, 0.0f, 1.0f), // Top left
	vec3(0.0f, 1.0f, 0.0f)  // Bottom right
);

vec3 right_face[4] = vec3[4](
	vec3(1.0f, 1.0f, 1.0f), // Top right
	vec3(1.0f, 0.0f, 0.0f), // Bottom left
	vec3(1.0f, 1.0f, 0.0f), // Bottom right
	vec3(1.0f, 0.0f, 1.0f)  // Top left
);

vec3 top_face[4] = vec3[4](
	vec3(0.0f, 1.0f, 0.0f),
	vec3(1.0f, 1.0f, 1.0f),
	vec3(1.0f, 1.0f, 0.0f),
	vec3(0.0f, 1.0f, 1.0f)
);

vec3 bottom_face[4] = vec3[4](
	vec3(0.0f, 0.0f, 1.0f),
	vec3(1.0f, 0.0f, 0.0f),
	vec3(1.0f, 0.0f, 1.0f),
	vec3(0.0f, 0.0f, 0.0f)
);

float shading_table[6] = float[6](0.8f, 0.8f, 0.6f, 0.6f, 1.0f, 0.4f);

// This is the order that vertices must be rendered
// because of the winding order required for backface culling
int indices[6] = int[6]( 0, 1, 2, 1, 0, 3 );

void main()
{
	// gl_VertexID = firstVertex + i
	// gl_DrawID * max_faces_in_bucket_per_lod * (local_vertex_id / 6)
	uint face_idx = gl_DrawID * 6666 + ((gl_VertexID % 39996) / 6);

	debug_color = vec4(0.2, 0.3, 0.8, 1.0);
	if (face_idx >= 953238) { // 953238 is 144th chunk start face offset
		debug_color = vec4(0.4, 0.7, 0.7, 1.0);
	}

	int faces_per_bucket = 6666;
	int chunks_in_row = 14;

//	for (int i = chunks_in_row; i >= 0; i--) {
//		if (face_idx < faces_per_bucket * chunks_in_row * i) {
//			float modif = 0.0;
//			if (i % 2) {
//				modif = chunks_in_row;
//			}
//			vec4 col = vec4(0.0, modif, i, 0.5) / float(chunks_in_row);
//			debug_color = col;
//		}
//	}

	uint target_face = faceStream.face[face_idx];
	
	uint x          = target_face          & 31u; // 5 bits
	uint y          = (target_face >> 5u)  & 31u; // 5 bits
	uint z          = (target_face >> 10u) & 31u; // 5 bits
	uint texture_id = (target_face >> 15u) & 31u; // 5 bits
	uint face_id    = (target_face >> 20u) & 7u;  // 3 bits
	uint vertex_id  = gl_VertexID % 6;

	vec3 vertex_pos = vec3(x, y, z);
	tex_coords = vec3(tex[indices[vertex_id]], texture_id);
	shading_values = shading_table[face_id];

	switch(face_id)
	{
		case 0u: // back 
			vertex_pos += back_face[indices[vertex_id]];
			break;
		case 1u: // front 
			vertex_pos += front_face[indices[vertex_id]];
			break;
		case 2u: // left 
			vertex_pos += left_face[indices[vertex_id]];
			break;
		case 3u: // right
			vertex_pos += right_face[indices[vertex_id]];
			break;
		case 4u: // top
			vertex_pos += top_face[indices[vertex_id]];
			break;
		case 5u: // bottom
			vertex_pos += bottom_face[indices[vertex_id]];
			break;
	}

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