#version 460 core
out vec4 frag_color;

uniform sampler2D water;

in vec2 tex_coords;

void main()
{
    //frag_color = vec4(0.0, 0.0,1.0,1.0);
    vec3 water_color = texture(water, tex_coords).rgb;
    water_color += vec3(0.0, 0.2, 0.0);
    frag_color = vec4(water_color, 0.5);
}