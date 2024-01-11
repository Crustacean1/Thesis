#version 420 core

struct Point{
	vec3 pos;
	float radius;
};

layout(std140, binding=3) uniform MetaPoints{
	Point m_points[128];
	float m_threshold;
	int m_point_count;
};

in vec3 v_position;
in vec3 v_direction;

out vec4 fCol;

float value(vec3 pos){
	float sum = 0;
	for(int  i = 0;i<m_point_count;i++){
		vec3 dist = pos - m_points[i].pos;
		sum += pow(m_points[i].radius, -dot(dist,dist));//, position - vPoints[i]);
	}
	return sum;
}


vec3 compute_derivative(vec3 position, vec3 center, float radius){
	vec3 delta = position - center;
	float dist_sqr = dot(delta,delta);
	float dist = sqrt(dist_sqr);
	return -2 * dist * pow(radius, -dist_sqr) * normalize(position - center);
}

vec3 compute_normal(vec3 pos){
	vec3 result = vec3(0,0,0);
	for(int i = 0;i<m_point_count;i++){
		result += compute_derivative(pos, m_points[i].pos, m_points[i].radius);
	}
	return normalize(result);
}

void main(){

	vec3 col = vec3(0);

	vec3 position = v_position;
	vec3 step = normalize(v_direction) * 0.05;

	for(int j = 0;j<256;j++){
		float sum = value(position);
		if(sum > m_threshold){
			col = vec3((position) * 0.1 + 0.5);
			col = vec3(max(dot(vec3(0,-1,0),compute_normal(position)), 0.1));
			break;
		}
		position += step;
	}
	fCol = vec4(col,1);
}
