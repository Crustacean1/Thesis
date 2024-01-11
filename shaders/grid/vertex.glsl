#version 420 core

layout(location = 0) in vec3 pos;

out vec3 vPos;

layout(std140, binding = 2) uniform CameraBlock{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

void main(){
    gl_Position = projection * view * model * vec4(pos,1.0);

		vec3 normal = (view * vec4(0,1,0,0)).xyz;
    vPos = (view * vec4(pos, 1.0)).xyz;
		vPos -= normal * dot(normal, vPos);
}
