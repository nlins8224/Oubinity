#version 460 core
out vec4 frag_color;

in vec2 tex_coords;

uniform sampler2D cloud_noise_0;
uniform vec2 u_resolution;
uniform float u_time;
uniform vec3 u_camera_front;
uniform vec3 u_camera_right;
uniform vec3 u_camera_up;
uniform vec3 u_camera_pos;
uniform vec2 u_mouse;
uniform float u_pitch;
uniform float u_yaw;

// uniform vec2 u_mouse;
// uniform float u_time;

// Clouds implementation: https://iquilezles.org/articles/dynclouds

vec3 rotx(vec3 p, float a){
    float s = sin(a), c = cos(a);
    return vec3(p.x, c * p.y - s * p.z, s * p.y + c * p.z);
}
vec3 roty(vec3 p, float a){
    float s = sin(a), c = cos(a);
    return vec3(c * p.x + s * p.z, p.y, -s * p.x + c * p.z);
}

float nmzHash(vec2 q)
{
    uvec2 p = uvec2(ivec2(q));
    p = p*uvec2(374761393U,22695477U) + p.yx;
    p.x = p.x*(p.y^(p.x>>15U));
    return float(p.x^(p.x >> 16U))*(1.0/float(0xffffffffU));
}

float noise(vec2 p) {
    vec2 ip = floor(p);
    vec2 fp = fract(p);
	vec2 u = fp*fp*(3.0-2.0*fp);
    return -1.0+2.0*mix( mix( nmzHash( ip + vec2(0.0,0.0) ), nmzHash( ip + vec2(1.0,0.0) ), u.x),
                mix(nmzHash( ip + vec2(0.0,1.0) ), nmzHash( ip + vec2(1.0,1.0)), u.x), u.y);
}

vec3 lgt = normalize(vec3(-1.0,0.1,.0));
vec3 hor = vec3(0);

float nz(vec2 p){return texture(cloud_noise_0, p*.01).x;}
mat2 m2 = mat2( 0.80,  0.60, -0.60,  0.80 );
float fbm(vec2 p, float d)
{	
	d = smoothstep(0.,100.,d);
    p *= .3/(d+0.2);
    float z=2.;
	float rz = 0.;
    p  -= u_time*0.2;
	for (float i = 1.0; i <= 5.0; i++)
	{
		rz += (sin(nz(p)*6.5)*0.5+0.5)*1.25/z;
		z *= 2.1;
		p *= 2.15;
        p += u_time*0.27;
        p *= m2;
	}
    return pow(abs(rz),2.-d);
}

vec3 render_sky_color(vec3 rd){
	//float sun_amount = max(dot(rd, SUN_DIR), 0.0);

	vec3  sky = mix(vec3(0.329, 0.608, 0.722), vec3(0.529, 0.808, 0.922), 1.0 - rd.y) / 1.25;
	//sky = sky + sun_color * min(pow(sun_amount, 1500.0) * 5.0, 1.0);
	//sky = sky + sun_color * min(pow(sun_amount, 10.0) * .6, 1.0);

	return sky;
}

vec4 clouds(vec3 ro, vec3 rd)
{    

    //float sun = clamp(dot(lgt,rd),0.0,1.0 );
    //hor = mix( 1.*vec3(0.70,1.0,1.0), vec3(1.3,0.55,0.15), 0.25+0.75*sun );
    //vec3 col = mix( vec3(0.5,0.75,1.), hor, exp(-(4.+ 2.*(1.-sun))*max(0.0,rd.y-0.05)) );
    //col *= 0.4;
	
   // col += 0.8*vec3(1.0,0.8,0.7)*pow(sun,512.0);
   // col += 0.2*vec3(1.0,0.4,0.2)*pow(sun,32.0);
    
    //col += 0.1*vec3(1.0,0.4,0.2)*pow(sun,4.0);
    vec3 col = render_sky_color(rd);
    
	float pt = (90.0 - ro.y) / rd.y; 
    vec3 bpos = ro + pt * rd;
    float dist = sqrt(distance(ro,bpos));
    float s2p = distance(bpos,lgt*100.);
    
    const float cls = 0.009;
    float bz = fbm(bpos.xz*cls,dist);
    float tot = bz;
    const float stm = .0;
    const float stx = 1.15;
    tot = smoothstep(stm,stx,tot);
    float ds = 2.;
    for (float i=0.;i<=3.;i++)
    {
        vec3 pp = bpos + ds*lgt;
        float v = fbm(pp.xz*cls,dist);
        v = smoothstep(stm,stx,v);
        tot += v;
        ds *= .14*dist;
    }

 
    col = mix(col,vec3(0.7)*0.3,pow(bz,1.5));
    tot = smoothstep(-7.5,-0.,1.-tot);
    vec3 sccol = mix(vec3(0.9),vec3(1.0),smoothstep(0.,900.,s2p));
    col = mix(col,sccol,1.-tot)*1.4;
    vec3 sncol = mix(vec3(1.1),vec3(1.4),smoothstep(0.,1200.,s2p));
    //float sd = pow(sun,10.)+.7;
    col += sncol*bz*bz*tot*tot*tot; //*sd;    
    return vec4(col,1.0);
}

float plane(vec3 ro, vec3 rd, vec3 c, vec3 u, vec3 v )
{
	vec3 q = ro - c;
	vec3 n = cross(u,v);
    return -dot(n,q)/dot(rd,n);
}

void main()
{	
//(fragCoord-.5*iResolution.xy)/iResolution.y;
    vec2 bp = (gl_FragCoord.xy - 1.5 * u_resolution.xy) / u_resolution.y;
    vec2 p  = bp;
	p.x *= u_resolution.x / u_resolution.y;

	vec3 ro = vec3(0.0, 0.0, 0.0);
    vec3 rd = normalize(vec3(p, -2.7));

    //float dir_y = ((u_camera_front.y / 2.0) + 0.5);
    //float dir_x = ((u_camera_up.x / 2.0) + 0.5);

    float MAX_PITCH = 90.0;
    float MAX_YAW = 359.0;
    float dir_y = u_pitch / MAX_PITCH;
    float dir_x = u_yaw / MAX_YAW;
    float PI = 3.14;

    rd = rotx(rd, dir_y + (PI / 16));
    rd = roty(rd, -dir_x * 2 * PI);
    vec3 col = vec3(0);
    
    float plane = plane(ro, rd, vec3(0.,-4.,0), vec3(1.,0.,0.), vec3(0.0,.0,1.0));
    if (plane < 5000. && plane > 0.)
    {
       col = vec3(0.46, 0.77, 0.9);
    } else {
       col = clouds(ro, rd).rgb;
    }

    //col = clouds(ro, rd).rgb;
	frag_color = vec4( col, 1.0 );
    //frag_color = texture(cloud_noise_0, tex_coords);
    //frag_color = vec4(0.5, 0.7, 0.2, 1.0);
}