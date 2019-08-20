#version 450 core

uniform sampler2D uTexture;
uniform vec3 uColor;

in vec2 vTexCoord;

out vec4 Color;

void main() {
	Color = texture(uTexture, vTexCoord) * vec4(uColor, 1.0);
}
