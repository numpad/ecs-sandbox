#version 450 core

uniform mat4 uProjection;
uniform mat4 uView;

layout(location = 0) in vec3 aPosition;

void main() {
	gl_Position = (uProjection * uView * vec4(aPosition, 1.0)).xyzw;
}
