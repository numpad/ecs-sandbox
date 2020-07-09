#version 450 core

uniform sampler2D uTexColor;
uniform sampler2D uTexPosition;
layout(binding = 2) uniform sampler2D uTexChoice;

in vec2 vPos;
in vec2 vTexCoord;
out vec4 Color;

void main() {
    vec4 albedo = texture(uTexColor, vTexCoord).rgba;
    vec4 position = texture(uTexPosition, vTexCoord).rgba;
    
	Color = vec4(albedo.rgb * max(0., min(position.y, 1.)), 1.0);
}
