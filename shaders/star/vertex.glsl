#version 460 core

layout(location = 0) in vec3 pos;

layout(std140,binding=1) uniform Camera{
	mat4 projection;
	mat4 view;
	vec3 c_pos;
};

layout(std140,binding=2) uniform Model{
	mat4 model;
};

out vec3 v_c_pos;
out vec3 v_pos;

void main(){
	vec4 model_pos = model * vec4(pos,1);
	gl_Position = projection * vec4((view * vec4(model_pos.xyz,0)).xyz,1);
	v_c_pos = /*c_pos*/- model[3].xyz;
	v_pos = pos;
}
