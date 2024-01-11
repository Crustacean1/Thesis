#version 420 core

in vec3 vPos;
//in float intensity;

out vec4 fCol;

//uniform sampler3D image;

void main() {
    fCol =  vec4(vec3(vPos) * 0.25 +0.75,1);
}
