#version 430 core
out vec4 frag_color;

in vec3 vertex_pos;

uniform float u_size;

float trunc_fallof( in float x, float m )
{
    x = 1.0 / ((x + 1.0) * (x + 1.0));
    m = 1.0 / ((m + 1.0) * (m + 1.0));
    return (x - m) / (1.0 - m);
}

void main()
{    
    vec3 st = vertex_pos / u_size;
    
    st = (st + 1.0) / 2.0;

    float y = smoothstep(0.0, 0.6, trunc_fallof(st.y, 1.3));
    vec3 c2 = vec3(0.0, 0.48, 0.51);
    vec3 c1 = vec3(1.0, 0.73, 0.21);
    vec4 color = vec4(mix(c2, c1, y), 1.0);

    frag_color = color;
}