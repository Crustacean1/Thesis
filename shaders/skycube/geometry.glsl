#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

layout (std140, binding=1) uniform  Camera {
	mat4 c_projection;
	mat4 c_view[6];
	vec3 c_pos;
};

in vec3 gPos[3];

out vec3 vPos;
out vec3 camera;

void main(){
	for(int i = 0;i<6;i++){
		for(int j = 0;j<3;j++){
			gl_Layer = i;
			gl_Position = c_projection * vec4((c_view[i] * vec4(gl_in[j].gl_Position)).xyz,1);
			vPos = gPos[j];
			camera = c_pos;
			EmitVertex();
		}
		EndPrimitive();
	}
}
