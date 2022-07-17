#version 450 core

#define WAVE_HEIGHT 0.2

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
uniform float uTime;
uniform vec2 uSize;

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
out vec3 vPosition;

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
	vec3 p = aPosition;
	p.y += noise(p.xz * uSize + vec2(uTime, uTime) * 0.3) * WAVE_HEIGHT;

	vPosition = (uModel * vec4(p, 1.0)).xyz;
	gl_Position = uProjection * uView * uModel * vec4(p, 1.0);
}

