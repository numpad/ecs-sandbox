#version 450 core

in vec3 vLightPosition;
in float vLightRadius;
in vec3 vLightColor;

layout(location = 0) out vec4 Color;

void main() {
	Color = vec4(vLightColor, 0.1);
}
