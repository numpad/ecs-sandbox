#version 450 core

uniform sampler2D uTexture;
uniform bool uUseImage;

in vec2 vTexCoord;

out vec4 Color;

void main() {
	if (uUseImage) {
		Color = texture(uTexture, vTexCoord);
	} else {
		Color = vec4(vTexCoord, 0.0, 1.0);
	}
}
