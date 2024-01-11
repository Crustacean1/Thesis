#version 460 core

layout(location = 0) in vec3 pos;

layout(std140,binding=1) uniform Camera{
	mat4 c_projection;
	mat4 c_view;
	vec4 c_pos;
};

out vec3 v_pos;

void main(){
	gl_Position = c_projection * vec4((c_view * vec4(pos,0)).xyz,1);
	v_pos = pos;
}
