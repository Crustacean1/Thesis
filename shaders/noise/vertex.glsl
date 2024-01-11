#version 460 core

layout(location = 0) in vec3 pos;

layout(std140,binding=2) uniform Camera{
	mat4 c_projection;
	mat4 c_view;
	vec3 c_pos;
};

layout(std140,binding=12) uniform SimplexNoise {
	vec4 directions[27];
	vec4 hash[128];
	int octaves;
	float brightness;
	float baseline;
	float scale;
	float displacement;
	float increment;
};

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+10.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec4 snoise(vec3 v)
  { 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod289(i); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 w = max(0.5 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  vec4 m = w * w;
  float value = 105.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
  vec3 gradient;
  gradient += m.x * m.x * p0;
  gradient += m.y * m.y * p1;
  gradient += m.z * m.z * p2;
  gradient += m.w * m.w * p3;
	
  gradient -= 6 * (m * w).x * dot(p0,x0) * x0;
  gradient -= 6 * (m * w).y * dot(p1,x1) * x1;
  gradient -= 6 * (m * w).z * dot(p2,x2) * x2;
  gradient -= 6 * (m * w).w * dot(p3,x3) * x3;

  return vec4(gradient, value);
}

vec4 fbm(vec3 pos){
	vec4 value = vec4(0);
	float m_scale = scale;
	float m_intensity = 0.5;

	for(int i = 0;i<octaves;i++){
		value += snoise(pos * m_scale) * m_intensity;

		m_scale *= 2;
		m_intensity /= increment;
	}
	return value;
}

vec3 displace(vec3 pos){
	vec3 g;
	float x = 0,y = 0,z = 0;
	//fbm(pos + vec3(0,0,displacement), x, g);
	//fbm(pos + vec3(0,displacement,0), y, g);
	//fbm(pos + vec3(displacement,0,0), z, g);

	return vec3(x,y,z);
}

//float distorted(vec3 pos){
	//vec3 grad;
	//float value;
	//return fbm(pos + displace(pos + displace(pos)));
//}


uniform mat4 model;
uniform float radius;

out vec3 v_pos;
out vec3 v_norm;

void main(){
	vec4 noise = fbm(pos);
	vec4 noise1 = fbm(pos + vec3(0.1,0,0));
	vec4 noise2 = fbm(pos + vec3(0,0.1,0));
	vec4 noise3 = fbm(pos + vec3(0,0,0.1));
	vec3 normie = vec3(noise1.w,noise2.w,noise3.w);

	vec3 z = normalize(pos);
	vec3 y = normalize(cross(z, z.yzx));
	vec3 x = normalize(cross(y,z));
	mat3 local = mat3(x,y,z);

	gl_Position = c_projection * c_view * model * vec4(pos * (radius + noise.w * 0.0),1);
	v_norm = local * (vec3(0,0,1) + normie * 0.4);
	v_pos = pos * radius;
}
