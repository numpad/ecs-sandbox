#version 450 core

in vec2 vTexCoord;

out vec4 Color;

void main() {

	Color = vec4(vTexCoord, 0.0, 1.0);
}
