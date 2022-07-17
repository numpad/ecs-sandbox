#version 450 core

uniform sampler2D uDepthTexture;
uniform sampler2D uPositionTexture;
uniform vec2 uScreenResolution;
uniform vec3 uCameraPosition;
uniform vec3 uBaseColor;
uniform float uTime;
in vec3 vPosition;

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Position;
layout(location = 2) out vec4 Normal;
layout(location = 3) out vec4 Depth;

float linearDepth() {
	const float uNearPlane = 1.0;
	const float uFarPlane = 100.0;
	float z = gl_FragCoord.z * 2.0 - 1.0;
	float d = (2.0 * uNearPlane * uFarPlane) / (uFarPlane + uNearPlane - z * (uFarPlane - uNearPlane));
	return d / uFarPlane;
}

float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 p){
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u * u * (3.0 - 2.0 * u);
	float res = mix(mix(rand(ip), rand(ip + vec2(1.0, 0.0)), u.x), mix(rand(ip + vec2(0.0, 1.0)), rand(ip + vec2(1.0, 1.0)), u.x), u.y);
	return res * res;
}

void main() {
	vec4 outColor = vec4(uBaseColor, 0.8);

	// foam...
	vec2 screenPos = gl_FragCoord.xy / uScreenResolution;
	vec3 d = vPosition - uCameraPosition;
	vec3 p = texture(uPositionTexture, screenPos).xyz - uCameraPosition;
	float dw = length(p - d);
	float foam = noise(vPosition.xz * 80.0 + vec2(uTime * 3.0f, 0.0)) * 0.05 + 0.02;
	// ...around edges
	outColor = mix(
		outColor,
		vec4(1.0),
		1.0 - step(foam, dw)
	);

	Color = outColor;
	Position = vec4(vPosition, 1.0);
	Depth = vec4(vec3(linearDepth()), 1.0);
}

