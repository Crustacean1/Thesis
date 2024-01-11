#version 460 core

in vec3 v_pos;
in vec3 v_norm;

out vec4 fCol;

uniform float time;

const float squash = (sqrt(3+1)-1)/3;
const float unsquash = (1 - (1/sqrt(3+1)))/3;

vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}


const vec3 orders[8][4] = {
	/*0*/{vec3(0,0,0), vec3(0,0,1), vec3(0,1,0),vec3(1,0,0)},
	/*1*/{vec3(0,0,0), vec3(0,0,0), vec3(0,0,0),vec3(0,0,0)}, // Impossible
	/*2*/{vec3(0,0,0), vec3(0,1,0), vec3(0,0,1),vec3(1,0,0)},
	/*3*/{vec3(0,0,0), vec3(0,1,0), vec3(1,0,0),vec3(0,0,1)},
	/*4*/{vec3(0,0,0), vec3(0,0,1), vec3(1,0,0),vec3(0,1,0)},
	/*5*/{vec3(0,0,0), vec3(1,0,0), vec3(0,0,1),vec3(0,1,0)},
	/*6*/{vec3(0,0,0), vec3(0,0,0), vec3(0,0,0),vec3(0,0,0)}, // Impossible
	/*7*/{vec3(0,0,0), vec3(1,0,0), vec3(0,1,0),vec3(0,0,1)},
};

float kernel(vec3 grad, vec3 position){
	return pow(max(0,0.5 - dot(position,position)),3) * dot(position, grad);
}

mat3 skew = mat3(1) + mat3(
	vec3(squash),vec3(squash),vec3(squash)
);
mat3 unskew = mat3(1) - mat3(
	vec3(unsquash),vec3(unsquash),vec3(unsquash)
);

float noise(vec3 position){
	vec3 corner = floor(skew * position);
	vec3 delta = position - (unskew * corner);

	int order = int(delta.x > delta.y);
	order <<= 1;
	order += int(delta.y > delta.z);
	order <<= 1;
	order += int(delta.x > delta.z);

	vec3 pos = corner;

	float value = 0;
	vec3 res;

	for(int i =0;i<4;i++){
		pos += orders[order][i];
		value += kernel(random3(pos), position - pos * unskew);
	}

	return value;
}

float fbm(vec3 position){
	float value = 0;
	float weight = 0.5;
	for(int i = 0;i<10;i++){
		value += noise(position);
		position *= 2;
		weight /= 1.8;
	}
	return  max(5 *value + 0.5,0);
}

void main(){
	//fCol = vec4(vec3(length(v_norm) ), 1);
	vec3 light = vec3(sin(time), cos(time),0);
	float intensity = max(0,dot(v_norm, light));
	float ridge = fbm(v_pos * 0.05);
	fCol = vec4(vec3((ridge * 0.5)* max(0.1,intensity) ), 1);
}
