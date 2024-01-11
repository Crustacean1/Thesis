#version 460 core

in vec3 vPos;
out vec4 fCol;

layout(std140,binding=2) uniform SimplexNoise {
	vec4 cloud_color;
	int iterations;
	int octaves;
	float density;
	float scatt;
	float scale;
	float increment;
	float offset;
	float brightness;
	float radius;
};

layout(std140,binding=3) uniform StarPoints{
	vec3 star_positions[16];
	vec3 star_colors[16];
	int star_count;
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

float snoise(vec3 v)
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
  vec4 m = max(0.5 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 105.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
  }

float fbm(vec3 pos){
	float value = 0;
	float m_scale = scale;
	float m_intensity = 1;

	vec3 displacement;
	for(int i = 0;i<octaves;i++){
		value += snoise(pos * m_scale) * m_intensity;
		m_scale *= 2;
		m_intensity /= increment;
	}
	return value;
}

float distorted(vec3 pos){
	float displacement = 0.1;
	vec3 disp = vec3(fbm(pos + vec3(0,0,displacement)),
				  	 fbm(pos + vec3(displacement,0,0)) ,
				  	 fbm(pos + vec3(0,displacement,0)));
	return fbm(pos + disp);
}

vec3 light_transmittance(vec3 position, vec3 light, int steps){
	vec3 transmittance = vec3(0);
	vec3 delta = (light - position) / steps;
	vec3 pos = position;
	float step = length(delta);

	for(int i = 0;i<steps;i++){
		transmittance += max(0,fbm(pos)) * step * cloud_color.xyz;
		pos += delta;
	}
	return transmittance;
}

float c_g = 0.6;
float scattering(float angle){
	return (1 - pow(c_g,2)) / ( 2 *pow(1 + pow(c_g,2) - 2 * c_g * angle, 3/2));
}

bool find_intersection(vec3 pos, vec3 direction, float radius, out vec2 intersection){
	float a = dot(direction,direction);
	float b = 2 * dot(pos, direction);
	float c = dot(pos,pos) - radius * radius;
	float delta = b * b - 4 * a * c;
	if(delta < 0){
		return false;
	}
	else{
		intersection = vec2(-b - sqrt(delta), -b + sqrt(delta)) / (2*a);
		return true;
	}
}

vec3 cast_ray(vec3 start, vec3 end, int steps){
	vec3 color = vec3(0);
	vec3 pos = start;
	vec3 delta = (end - start)/steps;
	vec3 transmittance = vec3(0);
	float step = length(delta);


	for(int l = 0;l<star_count;l++){
		float i = dot(normalize(delta), normalize(star_positions[l] - start));
		if(i > 0.99999){
			return star_colors[l];
		}
	}

	for(int i = 0;i<steps;i++){
		transmittance += max(0,fbm(pos)) * step * cloud_color.xyz;
		for(int l = 0;l<star_count;l++){
			float distance2 = dot((pos - star_positions[l]), (pos- star_positions[l]));
			float lumen = min(1 / distance2,5) * scattering(dot(normalize(-delta), normalize(pos - star_positions[l])));
			color += star_colors[l] * scatt * lumen * max(0,fbm(pos)) *  exp(-(transmittance + light_transmittance(pos, star_positions[l],steps)));
		}
		pos += delta;
	}
	return color;
}

void main(){
	vec3 end = normalize(vPos) * 5;
	vec3 color = cast_ray(vec3(0),end, iterations);
	fCol = vec4(color,1);
}
