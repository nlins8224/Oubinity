#version 330 core
out vec4 fragment_color;

uniform sampler2DArray texture_array_sampler;

in vec3 interpolated_tex_coords;

void main(void) {
	fragment_color = texture(texture_array_sampler, interpolated_tex_coords);
}