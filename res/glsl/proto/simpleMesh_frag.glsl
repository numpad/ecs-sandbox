#version 450 core

uniform sampler2D uTexDiffuse0;

in vec2 vTexCoords;

out vec4 Color;

void main() {
	vec4 pixel = texture(uTexDiffuse0, vTexCoords);
	Color = pixel;
}