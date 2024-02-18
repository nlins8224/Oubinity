#version 460 core

out vec2 tex_coords;

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_pos;

uniform mat4 view;
uniform mat4 projection;

uniform int water_area;
uniform float water_height;

void main()
{
	vec3 pos = in_pos;
	pos.xz *= water_area;
	pos.xz -= 0.33 * water_area;

	pos.y += water_height - 0.5;
	tex_coords = in_tex_pos * water_area;

	gl_Position = projection * view * vec4(pos, 1.0);
}