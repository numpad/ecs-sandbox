#version 450 core

in vec2 vTexCoord;

out vec4 Color;

void main() {
	Color = vec4(vTexCoord.xy, 0.5, 1.0);
}

