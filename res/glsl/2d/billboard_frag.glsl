#version 450 core

uniform vec3 uColor;
uniform sampler2D uTexture;

in vec2 vTexCoord;

out vec4 Color;

void main() {
	
	Color = texture(uTexture, vTexCoord);
}
