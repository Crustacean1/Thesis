#version 460 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

struct Vertex{
	vec4 position;
	vec4 normal;
};

struct Cube{
	Vertex vertices[12];
	int indices[16];
};

struct Blob{
	vec3 position;
	float padding;
	float intensity;
	float radius;
};

layout(std430, binding = 1) buffer spec{
	int edge_table[256];
	int triangle_connections[256][16];
	vec4 starting_position;
	float step_size;
};

layout(std430, binding = 2) buffer scalarField{
	Cube cubes[64 * 64 * 64];
};

layout(std430, binding = 3) buffer Steroid{
	Blob balls[128];
	int ball_count;
	float threshold;
	int octaves;
	float scale;
	float fractal;
	float base_brightness;
	float brightness;
	int iterations;
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
  vec4 m = max(0.5 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);

  vec4 v_grads = vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) );

  vec4 m2 = m*m;
  vec4 m3 = m2*m;
  vec4 m4 = m2*m2;

  vec3 g0 = m4.x * p0 - 8*m3.x * v_grads.x * x0;
  vec3 g1 = m4.y * p1 - 8*m3.y * v_grads.y * x1;
  vec3 g2 = m4.z * p2 - 8*m3.z * v_grads.z * x2;
  vec3 g3 = m4.w * p3 - 8*m3.w * v_grads.w * x3;

  return vec4((g0+g1+g2+g3), dot( m4, v_grads )) * 105.0;
}

vec4 fbm(vec3 pos, float inc){
	vec4 value = vec4(0);
	float m_scale = scale;
	float m_intensity = 0.5;

	vec3 displacement;
	vec3 octave_gradient;
	for(int i = 0;i<octaves;i++){
		value += snoise(pos * m_scale) * m_intensity;

		m_intensity /= inc;
		m_scale *= 2;
	}
	return value;
}

vec4 evaluate(vec3 pos){
	vec4 result = vec4(0);

	for(int i = 0;i<ball_count;i++){
		vec3 delta = (pos - balls[i].position)* balls[i].radius;
		float delta_2 = dot(delta, delta);
	 	float value = 1 / (1 + pow(delta_2,2));
		vec3 gradient = delta  * balls[i].radius * 4 * (delta_2 * pow(value, 2));

		result += vec4(gradient, value) * balls[i].intensity;
		result += fbm(pos, fractal) * brightness;
	}

	return result;
}

vec4 test_me(vec3 pos){
	vec3 n_pos = pos - vec3(1.5,1.5,1.5);
	return vec4(vec3(1),1 - dot(n_pos,n_pos) * 0.5);
}

vec3 cube_corners[8] = {vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0),
                    	vec3(1.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0),
                    	vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0),
                    	vec3(1.0, 1.0, 1.0), vec3(0.0, 1.0, 1.0)};

int edge_vertices[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
							{6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};


void main(){
	Cube cube;
	vec4 corners[8];
	int cube_type = 0;

	int index = int(gl_GlobalInvocationID.x + (gl_GlobalInvocationID.y + gl_GlobalInvocationID.z * 64) * 64);
	vec3 local_position = gl_GlobalInvocationID * step_size;
	
	for(int i = 0;i<8;i++){
		corners[7 - i] = evaluate(starting_position.xyz  +  local_position + cube_corners[7 - i] * step_size);
		cube_type <<= 1;
		cube_type += int(corners[7 - i].w > threshold);
	}

	int edge_map = edge_table[cube_type];
	if(edge_map != 0){
		for(int i = 0;i<12;i++){
			if((edge_map&1) == 1){
				float diff = corners[edge_vertices[i][0]].w - corners[edge_vertices[i][1]].w;
				float coeff = 0.5;
				if(abs(diff) > 1e-5){
					coeff = abs((corners[edge_vertices[i][0]].w - threshold)/ (diff));
				}
				cube.vertices[i].position = vec4(local_position + step_size * ((cube_corners[edge_vertices[i][0]] * (1-coeff)) + (cube_corners[edge_vertices[i][1]] * coeff)),0);
				cube.vertices[i].normal = vec4(normalize((corners[edge_vertices[i][0]] * (1-coeff) + corners[edge_vertices[i][1]] * coeff).xyz), 0);
			}
			edge_map>>=1;
		}
	}

	for(int i = 0;i < 16;i++){
		cube.indices[i] = triangle_connections[cube_type][i];
	}
	
	cubes[index] = cube;
}
