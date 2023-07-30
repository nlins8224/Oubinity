#version 430 core
out vec4 fragment_color;

uniform sampler2DArray texture_array_sampler;

in vec3 interpolated_tex_coords;
in float interpolated_shading_values;
in float visibility;
in float height;

float trunc_fallof( in float x, float m )
{
    x = 1.0 / ((x + 1.0) * (x + 1.0));
    m = 1.0 / ((m + 1.0) * (m + 1.0));
    return (x - m) / (1.0 - m);
}

void main(void) {
	vec3 fog_c1 = vec3(1.0, 0.73, 0.21);
	vec3 fog_c2 = vec3(0.0, 0.48, 0.51);
	vec3 fog_color = mix(fog_c2, fog_c1, trunc_fallof(height / 255, 1.3));
	fragment_color = vec4(fog_color, 1.0);
	//fragment_color = texture(texture_array_sampler, interpolated_tex_coords) * interpolated_shading_values;
	//fragment_color = mix(vec4(fog_color, 1.0), fragment_color, visibility);
}