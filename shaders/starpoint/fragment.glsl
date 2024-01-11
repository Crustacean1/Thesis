#version 460 core

in vec3 g_pos;
in vec4 g_color;
out vec4 f_col;

void main(){
	f_col = vec4(g_color.xyz, max(0,0.5 -  900000 * dot(g_pos, g_pos) ));
}
