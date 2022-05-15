#version 450 core

uniform mat4 uProjection;
uniform mat4 uView;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aPositionAndRadius;
layout (location = 2) in vec3 aColor;

out vec3 vColor;

void main() {
	vColor = aColor;
	gl_Position = uProjection * uView * vec4(aPositionAndRadius.xyz + aPosition * aPositionAndRadius.w, 1.0);
}
