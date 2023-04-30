#version 330 core

layout (location = 0) in uint in_xyzs;
layout (location = 1) in uint in_uvw;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 chunk_world_pos;

out vec3 interpolated_tex_coords;
out float interpolated_shading_values;
out float visibility;
out float height;

const float fog_density = 0.0035;
const float fog_gradient = 5.0;

void main()
{
	float x = float(in_xyzs  & 0x3Fu);
	float y = float((in_xyzs & 0xFC0u) >> 6u);
	float z = float((in_xyzs & 0x3F000u) >> 12u);

	interpolated_shading_values = float((in_xyzs & 0x1C0000u) >> 18u) / 5.0f;

	x += chunk_world_pos.x;
	y += chunk_world_pos.y;
	z += chunk_world_pos.z;
	
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