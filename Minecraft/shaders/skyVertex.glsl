#version 460 core
layout (location = 0) in vec3 in_pos;

out vec2 tex_coords;

void main()
{
 vec2 vertices[3] = vec2[3](
	vec2(-1,-1),
	vec2(3,-1),
	vec2(-1, 3));
 gl_Position = vec4(vertices[gl_VertexID], 1, 1);
 vec2 tex_coords = 0.5 * gl_Position.xy + vec2(0.5);
}