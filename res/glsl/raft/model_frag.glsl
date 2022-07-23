#version 450 core

in vec3 vPosition;
in vec3 vNormal;
in vec3 vColor;
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

void main() {
	Color = vec4(vColor, 1.0);
	Position = vec4(vPosition, 1.0);
	Normal = vec4(vNormal, 1.0);
	Depth = vec4(vec3(linearDepth()), 1.0);
}

