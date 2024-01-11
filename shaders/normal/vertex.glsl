#version 420 core
layout(location=0) in vec3 pos;
layout(location=1) in vec3 norm;

out vec4 vNorm;

layout (std140, binding = 1) uniform  Camera {
	mat4 c_projection;
	mat4 c_view;
	vec3 c_pos;
};

layout(std140, binding = 2) uniform Model{
	mat4 model;
};

void main(){
    gl_Position = c_projection * c_view * model * vec4(pos,1.0);
    vNorm = c_projection * c_view * model * vec4(pos + norm * 0.2, 1.0);
}
