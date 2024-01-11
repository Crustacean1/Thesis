#version 460 core

in vec3 v_pos;

out vec4 f_col;

layout(binding = 0) uniform samplerCube cubemap;

void main(){
	f_col = vec4(texture(cubemap,v_pos).xyz, 1);
	//f_col = vec4(1,0,0,1);
}
