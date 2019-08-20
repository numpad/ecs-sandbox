#version 450 core

uniform sampler2D uTexture;

in vec2 vTexCoord;
in vec3 vColor;

out vec4 Color;

void main() {
	Color = texture(uTexture, vTexCoord) * vec4(vColor, 1.0);
}
