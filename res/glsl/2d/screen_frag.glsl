#version 450 core

uniform sampler2D uTexColor;
uniform sampler2D uTexPosition;
layout(binding = 2) uniform sampler2D uTexChoice;
uniform bool uTexChoiceActive;

in vec2 vPos;
in vec2 vTexCoord;
out vec4 Color;

void main() {
    vec4 albedo = texture(uTexColor, vTexCoord).rgba;
    vec4 position = texture(uTexPosition, vTexCoord).rgba;
    
    vec3 outcolor;
    if (uTexChoiceActive) {
        outcolor = texture(uTexChoice, vTexCoord).rgb;
    } else {
        float brightness = clamp(position.y, 0., 1.);
        
        outcolor = albedo.rgb * smoothstep(.0, .94, brightness);
    }
    Color = vec4(outcolor, 1.0);
}
