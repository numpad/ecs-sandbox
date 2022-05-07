#version 450 core

uniform sampler2D uSource;
uniform sampler2D uTexCoords;

in vec2 vTexCoord;

out vec4 Color;

void main() {
	vec2 size = textureSize(uTexCoords, 0);

	vec4 texCoord = texture(uTexCoords, vTexCoord);
	vec4 outColor = texture(uSource, texCoord.xy * (size / 4.0));

	Color = vec4(outColor.rgb, outColor.a * texCoord.a);
}
