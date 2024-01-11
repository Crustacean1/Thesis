#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices=24) out;

layout(std140,binding=1) uniform Camera {
	mat4 c_projection;
	mat4 c_view[6];
	vec3 c_pos;
};

in vec4 v_color[1];

out vec3 g_pos;
out vec4 g_color;

const vec3 corners[] = {
	vec3(-0.004,0.004,0),
	vec3(0.004,0.004,0),
	vec3(-0.004,-0.004,0),
	vec3(0.004,-0.004,0),
};

void main(){
	vec3 x = normalize(cross(gl_in[0].gl_Position.xyz, gl_in[0].gl_Position.yzx));
	vec3 y = normalize(cross(x, gl_in[0].gl_Position.xyz));

	mat3 local = mat3(x,y,normalize(gl_in[0].gl_Position.xyz));

	vec3 pos = gl_in[0].gl_Position.xyz;

	vec4 col = vec4(1,0.5,0.5,1);

	for(int j = 0;j<6;j++){
		for(int i = 0;i<4;i++){
			gl_Layer = j;
			gl_Position = c_projection * vec4((c_view[j] * vec4(pos + local * corners[i], 0)).xyz, 1);
			g_pos = corners[i];
			g_color = v_color[0];
			EmitVertex();
		}
		EndPrimitive();
	}
}
