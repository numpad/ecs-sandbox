#version 450 core

uniform mat4 uProjection;
uniform mat4 uView;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aPositionAndRadius;
layout (location = 2) in vec3 aColor;

out vec3 vLightPosition;
out float vLightRadius;
out vec3 vLightColor;

void main() {
	vLightColor = aColor;
	vLightPosition = aPositionAndRadius.xyz;
	vLightRadius = aPositionAndRadius.w;
	gl_Position = uProjection * uView * vec4(aPositionAndRadius.xyz + aPosition * aPositionAndRadius.w, 1.0);
}
