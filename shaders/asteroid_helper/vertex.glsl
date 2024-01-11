#version 460 core

layout(location = 0) in vec3 pos;

layout(std140,binding=1) uniform Camera{
	mat4 c_projection;
	mat4 c_view;
	vec3 c_pos;
};

layout(std140, binding = 2) uniform Model {
	mat4 model;
};

out vec3 v_c_pos;
out vec3 v_c_dir;

void main(){
	gl_Position = c_projection * c_view * model * vec4(pos, 1);
	v_c_pos = c_pos - model[3].xyz;
	v_c_dir = pos - v_c_pos;
}
