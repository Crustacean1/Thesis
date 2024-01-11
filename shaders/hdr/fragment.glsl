#version 460 core

in vec2 v_tex;

layout(std140, binding=2) uniform Dimensions {
	float width;
	float height;
};

layout(binding = 0) uniform sampler2D hdr_texture;

out vec4 f_col;

void main(){
	vec3 hdrColor = texture(hdr_texture, v_tex).xyz;
	const int radius = 5;
	//for(int x = 0;x<radius;x++){
		//for(int y = 0;y<radius;y++){
			//vec2 displacement = (vec2(x,y) - 0.5 * vec2(radius) * vec2(1/width, 1/height));
			//vec3 localColor = texture(hdr_texture, v_tex + displacement).xyz;
			//hdrColor += max(dot(localColor, localColor) - 2,0)  * 0.1 * localColor * exp(-dot(displacement, displacement));
		//}
	//}
    const float gamma = 2.2;
    vec3 mapped = vec3(1) - exp(-hdrColor * 0.3);
    mapped = pow(mapped, vec3(1.0 / gamma));

	f_col = vec4(hdrColor, 1);
}
