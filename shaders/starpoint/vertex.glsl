#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;


out vec4 v_color;

void main(){
	//gl_Position = c_projection * vec4((c_view * vec4(star_pos.xyz,0)).xyz, 1);
	//gl_Position = vec4(vec3(0,1,0),1);
	gl_Position =  vec4(pos.xyz,0);
	v_color = color;
}
