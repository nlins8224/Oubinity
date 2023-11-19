#version 460 core
layout (location = 0) in uint in_xyzs;
layout (location = 1) in uint in_uvw;

out vec3 interpolated_tex_coords;
out float interpolated_shading_values;
out int draw_id_debug;
out float visibility;
out float height;

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

void main()
{
	draw_id_debug = gl_DrawID;

	float x = float(in_xyzs  & 0x3Fu);
	float y = float((in_xyzs & 0xFC0u) >> 6u);
	float z = float((in_xyzs & 0x3F000u) >> 12u);
	interpolated_shading_values = float((in_xyzs & 0x1C0000u) >> 18u) / 5.0f;

	x *= lod.block_size[gl_DrawID];
	y *= lod.block_size[gl_DrawID];
	z *= lod.block_size[gl_DrawID];

	/*
	Surface height is determined first, block is placed after calculations.
	This means that larger blocks will not align with smaller blocks. Larger block bottom face
	will be neighbour of smaller block top face.
	*/
	y -= lod.block_size[gl_DrawID] - 1;

	x += chunkInfo.chunk_pos[gl_DrawID].x;
	y += chunkInfo.chunk_pos[gl_DrawID].y;
	z += chunkInfo.chunk_pos[gl_DrawID].z;

	gl_Position = projection * view * model * vec4(x, y, z, 1.0);

	float u = float(in_uvw &  0x3Fu);
	float v = float((in_uvw & 0xFC0u) >> 6u);
	float w = float((in_uvw & 0x3F000u) >> 12u);

	interpolated_tex_coords = vec3(u, v, w);

	// Calculate fog
	vec4 pos_to_cam = view * vec4(x, y, z, 1.0);
	float fog_distance = length(pos_to_cam.xyz);
	visibility = exp(-pow((fog_distance * fog_density), fog_gradient));
	visibility = clamp(visibility, 0.0, 1.0);

    height = y;
}