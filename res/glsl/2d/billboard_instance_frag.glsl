#version 450 core

uniform sampler2D uTexture;

in vec2 vTexCoord;
in vec3 vColor;

out vec4 Color;

void main() {
	vec4 pixel = texture(uTexture, vTexCoord);
	if (pixel.r > 0.9 && pixel.g < 0.1 && pixel.b > 0.9)
		pixel = vec4(vColor, pixel.a);
	
	Color = pixel;
}
