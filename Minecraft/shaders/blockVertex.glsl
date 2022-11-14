#version 330 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 tex_coords;
layout(location = 2) in float shading_values;

out vec3 interpolated_tex_coords;
out float interpolated_shading_values;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	interpolated_tex_coords = tex_coords;
	interpolated_shading_values = shading_values;
	gl_Position = projection * view * model * vec4(vertex_position, 1.0);
}