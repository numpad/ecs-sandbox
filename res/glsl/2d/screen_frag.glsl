#version 450 core

uniform sampler2D uTexColor;
uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
uniform sampler2D uTexDepth;
uniform float uTime;

in vec2 vPos;
in vec2 vTexCoord;
out vec4 Color;

/*vec3 WorldPosFromDepth(float depth) {
	float z = depth * 2.0 - 1.0;

	vec4 clipSpacePosition = vec4(vTexCoord * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = projMatrixInv * clipSpacePosition;

	// Perspective division
	viewSpacePosition /= viewSpacePosition.w;

	vec4 worldSpacePosition = viewMatrixInv * viewSpacePosition;

	return worldSpacePosition.xyz;
}*/

void main() {
    vec4 albedo = texture(uTexColor, vTexCoord).rgba;
    vec4 position = texture(uTexPosition, vTexCoord).rgba;
    vec4 normal = texture(uTexNormal, vTexCoord).rgba;
	vec4 depth = texture(uTexDepth, vTexCoord).rgba;
    

	float brightness = clamp(position.y*.5+.5, 0., 1.);
	brightness = smoothstep(.0, .89, brightness);
	brightness = clamp(brightness, 0.1, 0.8);
	vec3 outcolor = albedo.rgb * brightness;
	
    Color = vec4(outcolor, 1.0);
}
