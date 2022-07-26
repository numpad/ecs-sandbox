#version 450 core

in vec3 vColor;
out vec4 Color;

void main() {
	Color = vec4(vColor, 1.0);
}

