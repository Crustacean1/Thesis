#version 460 core

layout(location = 0) in vec3 pos;

out vec3 gPos;

void main(){
	gl_Position = vec4(pos,0);
	gPos = pos;
}
