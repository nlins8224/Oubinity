#version 460 core
layout (location = 0) in uint in_xyzs;
layout (location = 1) in uint in_uvw;

out vec3 interpolated_tex_coords;
out float interpolated_shading_values;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (std430, binding = 0) readonly buffer ChunkInfo
{
	vec4 chunk_pos[];
} chunkInfo;

void main()
{
	float x = float(in_xyzs  & 0x3Fu);
	float y = float((in_xyzs & 0xFC0u) >> 6u);
	float z = float((in_xyzs & 0x3F000u) >> 12u);
	interpolated_shading_values = float((in_xyzs & 0x1C0000u) >> 18u) / 5.0f;

	x += chunkInfo.chunk_pos[gl_DrawID].x;
	y += chunkInfo.chunk_pos[gl_DrawID].y;
	z += chunkInfo.chunk_pos[gl_DrawID].z;

	gl_Position = projection * view * model * vec4(x, y, z, 1.0);

	float u = float(in_uvw &  0x3Fu);
	float v = float((in_uvw & 0xFC0u) >> 6u);
	float w = float((in_uvw & 0x3F000u) >> 12u);

	interpolated_tex_coords = vec3(u, v, w);
}