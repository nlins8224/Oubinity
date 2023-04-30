#version 330 core
out vec4 frag_color;

in vec3 tex_coords;

uniform samplerCube u_skybox;

void main()
{    
    frag_color = texture(u_skybox, tex_coords);
}