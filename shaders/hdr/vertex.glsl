#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 2) in vec2 tex;

layout(std140,binding=1) uniform Camera{
	mat4 c_projection;
	mat4 c_view;
	vec4 c_pos;
};

out vec2 v_tex;

void main(){
	gl_Position = vec4(pos,1);
	v_tex = tex;
}
