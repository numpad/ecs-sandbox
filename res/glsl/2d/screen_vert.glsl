#version 450 core

uniform vec2 uScreenSize;

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec2 vPos;
out vec2 vTexCoord;

void main() {
	vPos = aPosition;
	vTexCoord = aTexCoord;
    gl_Position = vec4(aPosition.xy, 0.0, 1.0);
}
