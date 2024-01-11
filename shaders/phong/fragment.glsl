#version 420 core

in vec3 vPos;
in vec3 vNorm;
in vec2 vTex;
in vec4 lPos;

out vec4 fCol;

struct DirectionalLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
};

uniform sampler2D image;
uniform sampler2D shadow_map;

uniform int dir_light_count;
uniform DirectionalLight dir_lights[16];

vec3 compute_dir_light(DirectionalLight dir_light){
	vec3 light_ray = normalize(dir_light.direction);
	vec3 norm = normalize(vNorm);
	float diffuse = max(dot(light_ray, norm),0);

	float specular = pow(max(dot(normalize(vPos),reflect(light_ray, norm)),0),128);
	return dir_light.ambient + dir_light.diffuse * (diffuse +  specular);
}

vec3 get_light_persp(vec4 in_light){
	vec3 light = in_light.xyz / in_light.w;
	light = light * 0.5 + 0.5;
	return light;
}

void main(){

	vec3 col = vec3(0);
	fCol = 0.000001 * texture(shadow_map, 1 - lPos.xy).r * vec4(1,1,1,1);
	for(int i = 0;i<dir_light_count;i++){
		col += compute_dir_light(dir_lights[i]);
	}

	vec3 light = get_light_persp(lPos);

	if(texture(shadow_map,light.xy).r > light.z ){
		fCol = texture(image, vTex) * vec4(col,1);
	}
	else{
		fCol = texture(image, vTex) * vec4(0.1,0.1,0.1,1);
	}
}
