#version 450 core

uniform vec3 uColor;

out vec4 FragColor;

void main() {
	FragColor = vec4(uColor.rgb, 1.0);
}
