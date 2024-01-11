#version 420 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;

uniform mat4 model;

layout(std140,binding=2) uniform Camera{
	mat4 projection;
	mat4 view;
};

out vec3 v_pos;
out vec3 v_norm;

void main(){
	gl_Position = projection * view * model * vec4(pos,1);
	v_pos = pos;
	v_norm = norm;
}
