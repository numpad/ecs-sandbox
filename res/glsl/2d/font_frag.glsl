#version 450 core

uniform sampler2D uTexture;
uniform vec3 uTextColor;

in vec2 vTexCoord;

out vec4 Color;

void main() {
	float alpha = texture(uTexture, vTexCoord).r;
	Color = vec4(uTextColor, alpha);
}
