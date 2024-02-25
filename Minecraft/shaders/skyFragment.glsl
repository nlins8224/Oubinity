#version 460 core
out vec4 frag_color;

in vec2 tex_coords;

uniform sampler2D cloud_noise_0;

void main()
{
    //frag_color = texture2D(cloud_noise_0, tex_coords);
    frag_color = vec4(0.9, 0.3, 0.3, 1.0);
}