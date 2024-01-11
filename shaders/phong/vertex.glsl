#version 420 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;

uniform mat4 light_matrix;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 vPos;
out vec4 lPos;
out vec2 vTex;
out vec3 vNorm;

void main(){
	vNorm = (view * model * vec4(norm, 0)).xyz;
	vTex = tex;
	vPos = (view * model * vec4(pos,1)).xyz;
	lPos = light_matrix * vec4(( vec4(pos,1)).xyz,1);
	gl_Position = projection * view * model * vec4(pos,1);
}
