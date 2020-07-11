#version 450 core

uniform sampler2D uTexColor;
uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
layout(binding = 5) uniform sampler2D uTexChoice;
uniform bool uTexChoiceActive;

in vec2 vPos;
in vec2 vTexCoord;
out vec4 Color;

void main() {
    vec4 albedo = texture(uTexColor, vTexCoord).rgba;
    vec4 position = texture(uTexPosition, vTexCoord).rgba;
    vec4 normal = texture(uTexNormal, vTexCoord).rgba;
    
    vec3 outcolor;
    if (uTexChoiceActive) {
        outcolor = texture(uTexChoice, vTexCoord).rgb;
    } else {
        float brightness = clamp(position.y*.5+.5, 0., 1.);
        brightness = smoothstep(.0, .89, brightness);
        brightness = clamp(brightness, 0.1, 0.8);
        outcolor = albedo.rgb * brightness;
    }
    Color = vec4(outcolor, 1.0);
}
