#version 450 core

uniform mat4 uProjection;

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

void main() {
	gl_Position = uProjection * vec4(aPosition, 0.0, 1.0);
	vTexCoord = aTexCoord;
}
