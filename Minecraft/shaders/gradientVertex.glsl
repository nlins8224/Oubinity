#version 330 core
layout (location = 0) in vec3 pos;

out vec3 vertex_pos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vertex_pos = pos;
    gl_Position = projection * view * vec4(pos, 1.0);
}  