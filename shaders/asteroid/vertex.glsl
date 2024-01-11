#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;

layout (std140, binding = 1) uniform  Camera {
	mat4 c_projection;
	mat4 c_view;
	vec3 c_pos;
};

layout(std140, binding = 2) uniform Model{
	mat4 model;
};

out vec3 v_norm;
out vec3 v_pos;

void main(){
	gl_Position =  c_projection * c_view * model * vec4(pos, 1);
	v_norm =  (model * vec4(normalize(norm),0)).xyz;
	v_pos = pos;
}
