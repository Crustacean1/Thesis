#version 420 core

in vec3 vPos;

out vec4 fCol;

void main(){
    float distance = max(1 - length(vPos) / 35.0, 0);
    fCol = vec4( vec3(0.8,0.8,0.8),distance);
}
