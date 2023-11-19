#version 460 core
out vec4 fragment_color;

uniform sampler2DArray texture_array_sampler;

in vec3 interpolated_tex_coords;
in float interpolated_shading_values;
in float visibility;
in float height;
flat in int draw_id_debug;

void main(void) {
		
	vec3 fog_color = vec3(0.75, 0.75, 0.75);
	fragment_color = texture(texture_array_sampler, interpolated_tex_coords) * interpolated_shading_values;
	fragment_color = mix(vec4(fog_color, 1.0), fragment_color, visibility);

	//float debug_color = draw_id_debug / 5.0;
	//fragment_color = vec4(debug_color, debug_color, 0.0, 0.0);

}