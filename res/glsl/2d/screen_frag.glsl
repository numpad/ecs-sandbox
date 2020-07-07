#version 450 core

uniform sampler2D uScreen;

in vec2 vPos;
in vec2 vTexCoord;
out vec4 Color;

void main() {
	Color = vec4(texture(uScreen, vTexCoord).rgb, 1.0);
}
