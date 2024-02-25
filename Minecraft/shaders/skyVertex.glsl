#version 460 core

out vec2 tex_coords;

layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_tex_pos;

void main()
{
// vec2 vertices[3] = vec2[3](
//	vec2(-1,-1),
//	vec2(3,-1),
//	vec2(-1, 3));
 //gl_Position = vec4(vertices[gl_VertexID], 1, 1);
 //vec2 tex_coords = (0.5 * gl_Position.xy) + vec2(0.5);

 	vec2 pos = in_pos;
    tex_coords = in_tex_pos;

    gl_Position = vec4(pos, 1, 1);
}