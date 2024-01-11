#version 420 core

layout(triangles) in;
layout(line_strip, max_vertices=6) out;

in vec4 vNorm[];
out float gCol;

void main(){
    for(int i = 0;i<3;i++){
        gl_Position = gl_in[i].gl_Position;
        gCol = 1;
        EmitVertex();
        gl_Position = vNorm[i];
        gCol = 0;
        EmitVertex();
        EndPrimitive();
    }
}
