#version 460 core

in vec3 v_c_pos;
in vec3 v_pos;

out vec4 f_col;

const float squash = (sqrt(3+1)-1)/3;
const float unsquash = (1 - (1/sqrt(3+1)))/3;

struct Light{
	vec3 direction;
	vec3 color;
};

layout(std140,binding=3) uniform SimplexNoise {
	vec4 colors[4];
	vec4 scattering;
	Light light;
	float density_falloff;
	float planet_radius;
	int octaves;
	float intensity;
	float offset;
	float scale;
	float increment;
	float surface_increment;
	float surface_base;
	float surface_factor;
};

vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}


vec3 gradient(vec3 pos){
	int x = (int(pos.x) + 14) & 511;
	int y = (int(pos.y) + 14) & 511;
	int z = (int(pos.z) + 14) & 511;
	return vec3(1);
}

const vec3 orders[8][4] = {
	/*0*/{vec3(0,0,0), vec3(0,0,1), vec3(0,1,0),vec3(1,0,0)},
	/*1*/{vec3(0,0,0), vec3(0,0,0), vec3(0,0,0),vec3(0,0,0)}, // Impossible
	/*2*/{vec3(0,0,0), vec3(0,1,0), vec3(0,0,1),vec3(1,0,0)},
	/*3*/{vec3(0,0,0), vec3(0,1,0), vec3(1,0,0),vec3(0,0,1)},
	/*4*/{vec3(0,0,0), vec3(0,0,1), vec3(1,0,0),vec3(0,1,0)},
	/*5*/{vec3(0,0,0), vec3(1,0,0), vec3(0,0,1),vec3(0,1,0)},
	/*6*/{vec3(0,0,0), vec3(0,0,0), vec3(0,0,0),vec3(0,0,0)}, // Impossible
	/*7*/{vec3(0,0,0), vec3(1,0,0), vec3(0,1,0),vec3(0,0,1)},
};

float kernel(vec3 grad, vec3 position){
	return pow(max(0,0.5 - dot(position,position)),3) * dot(position, grad);
}

mat3 skew = mat3(1) + mat3(
	vec3(squash),vec3(squash),vec3(squash)
);
mat3 unskew = mat3(1) - mat3(
	vec3(unsquash),vec3(unsquash),vec3(unsquash)
);

float noise(vec3 position){
	vec3 corner = floor(skew * position);
	vec3 delta = position - (unskew * corner);

	int order = int(delta.x > delta.y);
	order <<= 1;
	order += int(delta.y > delta.z);
	order <<= 1;
	order += int(delta.x > delta.z);

	vec3 pos = corner;

	float value = 0;
	vec3 res;
	//return vec3((value - 0.002)/ 0.025);

	for(int i =0;i<4;i++){
		pos += orders[order][i];
		value += kernel(random3(pos), position - pos * unskew);
	}

	return (value * intensity) + offset;
}

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

float fbm(vec3 pos, float inc){
	float value = 0;
	float m_scale = scale;
	float m_intensity = 0.5;

	vec3 displacement;
	for(int i = 0;i<octaves;i++){
		value += snoise(pos * m_scale) * m_intensity;
		m_scale *= 2;
		m_intensity /= inc;
	}
	return value;
}

vec4 map_to_color(float v){
	vec4 a = colors[max(0,min(3,int(floor(v*4))))];
	vec4 b = colors[max(0,min(3,1 + int(floor(v*3))))];
	float inter = smoothstep(0,1,fract(v*3));
	//float inter = 1/ (1 + exp(v - floor(v *3)))
	//return a * (1-inter) + b * (inter);
	return a;
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

float atmosphere_radius = 1.0;

float pi = 3.1415;
float air = 1.000293;
float base_height = 0.25;
float light_intensity = 1;
//float K = (2 * pi * pi * pow(air * air - 1, 2))/(3*)

float rayleigh_phase(float cosinus){
	return 0.75 * (1 + cosinus * cosinus);
}

float optical_density(vec3 position){
	return exp(- density_falloff * (length(position) - planet_radius)/base_height);
}

vec3 optical_depth(vec3 start, vec3 end){
	int samples = 64;
	float value = 0;
	vec3 pos = start;

	vec3 delta = (end - start) / (samples -1);
	float step = length(delta);


	for(int i = 0;i < samples;i++){
		value += optical_density(pos) * step;
		pos += delta;
	}

	return 4 * pi * scattering.xyz * value;
}

bool find_light_entry(vec3 pos, vec3 light_direction, out vec3 light_position){
	vec2 intersection;
	if(!find_intersection(pos, -light_direction, planet_radius, intersection) || intersection.x < 0){
		find_intersection(pos, -light_direction, atmosphere_radius, intersection);
		light_position = pos + light_direction * intersection.x;
		return true;
	}

	return false;
}

vec4 atmosphere(vec3 start, vec3 end){
	int samples = 32;
	vec3 pos = start;
	vec3 value = vec3(0);

	vec3 light_entry;
	vec3 delta = (end - start) / (samples -1);
	float step = length(delta);

	for(int i = 0;i<samples;i++){
		if(find_light_entry(pos, -light.direction, light_entry)){
			vec3 depth = optical_depth(pos, start) + optical_depth(pos, light_entry);
			value.x += step * optical_density(pos) * exp(-depth.x);
			value.y += step * optical_density(pos) * exp(-depth.y);
			value.z += step * optical_density(pos) * exp(-depth.z);
		}
		pos += delta;
	}
	vec3 light = value * scattering.xyz * 8 * rayleigh_phase(dot(normalize(end-start), normalize(-light.direction)));
	return vec4(light, min(1,length(light)));
}

vec4 planet(vec3 start, vec3 end){
	int samples = 32;
	vec3 pos = start;
	vec3 value = vec3(0);

	vec3 light_entry;
	vec3 delta = (end - start) / (samples -1);
	float step = length(delta);

	for(int i = 0;i<samples;i++){
		if(find_light_entry(pos, -light.direction, light_entry)){
			vec3 depth = optical_depth(pos, start) + optical_depth(pos, light_entry);
			value.x += step * optical_density(pos) * exp(-depth.x);
			value.y += step * optical_density(pos) * exp(-depth.y);
			value.z += step * optical_density(pos) * exp(-depth.z);
		}
		pos += delta;
	}

	value = value * scattering.xyz * 8 * rayleigh_phase(dot(normalize(end-start), -normalize(light.direction)));
	float outline = fbm(end, surface_increment);
	float noise = fbm(end, increment) * surface_factor + surface_base;
	//float noise = fbm(end, increment) * 0.05 + 0.8;
	vec3 color = noise * map_to_color(outline * 0.5 + 0.5).xyz * max(0,dot(light.direction, normalize(end))) * exp(-optical_depth(pos, start));
	return vec4(color + value,1);
 	//return vec4(map_to_color(outline).xyz * outline,1);
}

void main(){
	vec3 direction = v_pos - v_c_pos;
	vec2 intersection;
	vec2 p_intersection;

	f_col = vec4(0);
	if(find_intersection(v_c_pos, direction, 1, intersection)){
		vec3 start = v_c_pos + direction * max(0,intersection.x);
		vec3 end = v_c_pos + direction * intersection.y;
		if(find_intersection(v_c_pos, direction, planet_radius, p_intersection) && p_intersection.x > 0){
			f_col = planet(start, v_c_pos + direction * p_intersection.x);
		}
		else{
			f_col = atmosphere(start, end);
		}
	}
	else{
		discard;
	}
}
