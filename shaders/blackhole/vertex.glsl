#version 420 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;

layout(std140,binding=1) uniform Camera{
	mat4 c_projection;
	mat4 c_view;
	vec3 c_pos;
};

layout(std140, binding = 2) uniform Model {
	mat4 model;
};

out vec3 v_pos;
out vec3 v_c_pos;

void main(){
	gl_Position = c_projection * vec4((c_view * vec4((model * vec4(pos, 1)).xyz, 0)).xyz,1);
	v_c_pos = /*c_pos*/ -(vec4(model * vec4(0,0,0,1)).xyz);
	v_pos = (model * vec4(pos,0)).xyz;
}
