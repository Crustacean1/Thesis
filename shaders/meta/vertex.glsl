#version 420 core

layout(location = 0) in vec3 pos;

layout(std140, binding=2) uniform Camera{
	mat4 c_projection;
	mat4 c_view;
	vec3 c_pos;
};

uniform mat4 model;

out vec3 v_position;
out vec3 v_direction;

void main(){
	gl_Position = c_projection * c_view * model * vec4(pos, 1);
	vec3 cam_pos = model[3].xyz - c_pos;

	v_position = pos;
	v_direction = pos - (c_pos - model[3].xyz);
}
