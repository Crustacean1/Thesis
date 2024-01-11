#version 420 core

in vec3 center;
in vec3 v_pos;
in vec3 v_c_pos;

out vec4 f_col;

layout(binding = 0) uniform samplerCube cubemap;

layout(std140, binding = 3) uniform BlackHole{
	float radius;
	float step;
	float strength;
	int iterations;
};

void main(){
	vec3 cam_dir = normalize(v_pos - v_c_pos);
	float intensity = 2 - sqrt(dot(v_c_pos,v_c_pos) - pow(dot(v_c_pos, cam_dir),2));
	vec3 sphere_intersect = v_c_pos + cam_dir * (dot(-v_c_pos,cam_dir) - intensity * 0.5);

	float horizon;
	if(intensity > 0){
		vec3 pos = sphere_intersect;
		vec3 dir = cam_dir;
		float l_step = step * 0.001;
		for(int i = 0; i<iterations; i++){
			float dist = length(pos);
			dir -= l_step * strength * (pos / (pow(dist * radius,4)));
			pos += dir * l_step;
		}
		if(dot(pos,pos) > 0.1){
			f_col = vec4(texture(cubemap, dir).xyz, 1);
		}
		else{
			f_col = vec4(0,0,0,1);
		}
	}
	else{
		f_col = vec4(0);
	}
	//f_col = vec4(1);
}
