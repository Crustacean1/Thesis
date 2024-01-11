#version 420 core

layout(location = 0) in vec3 pos;

layout(std140, binding=1) uniform Camera{
	mat4 c_projection;
	mat4 c_view;
	vec3 c_pos;
};

layout(std140, binding=2) uniform Model{
	mat4 model;
};

out vec3 vPos;

void main(){
    gl_Position = c_projection * c_view * model * vec4(pos,1);
	vPos = pos;
}
