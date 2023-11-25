#version 460 core
out vec4 fragment_color;

uniform sampler2DArray texture_array_sampler;

in vec3 tex_coords;
in float shading_values;
in float visibility;
in float height;

void main(void) {
	vec3 fog_color = vec3(0.75, 0.75, 0.75);
	fragment_color = texture(texture_array_sampler, tex_coords) * shading_values;
	fragment_color = mix(vec4(fog_color, 1.0), fragment_color, visibility);
}