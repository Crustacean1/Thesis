#version 420 core

layout(location = 0) in vec3 pos;

uniform mat4 light_matrix;

void main(){
	gl_Position = light_matrix * vec4(pos,1);
}
