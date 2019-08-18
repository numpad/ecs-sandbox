#version 450 core

uniform sampler2D uTexture;

in vec2 vTexCoord;

out vec4 Color;

void main() {
	Color = texture(uTexture, vTexCoord);
}
