#version 450 core

uniform sampler2D uTextureSide;
uniform sampler2D uTextureTopdown;
uniform float uTextureSideScale;
uniform float uTextureTopdownScale;
uniform float uTime;

in vec3 vPos;
in vec3 vNormal;
flat in uvec2 vTexture;

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Position;
layout(location = 2) out vec4 Normal;
layout(location = 3) out vec4 Depth;

vec3 asymTriplanarBlend(vec3 normal) {
	float yBlendEdge = 0.7;
	
	vec3 blend = vec3(0.0);
	// sides only
	vec2 xzBlend = abs(normalize(normal.xz));
	blend.xz = max(vec2(0.0), xzBlend - yBlendEdge);
	blend.xz /= max(0.00001, dot(blend.xz, vec2(1.0, 1.0)));
	// top
	blend.y = clamp((abs(normal.y) - yBlendEdge * 1.02) * 80.0, 0.0, 1.0);
	blend.xz *= (1.0 - blend.y);
	return blend;
}

vec3 triplanarBlend(vec3 normal) {
	vec3 blend = abs(normal);
	blend = normalize(max(blend, 0.000001));
	if (blend.y < 0.8) blend.y *= 0.1;
	
	blend = normalize(blend);
	float b = blend.x + blend.y + blend.z;
	return blend / b;
}

vec4 triplanarTexture(vec3 pos, vec3 blend) {
	vec4 xtex = texture(uTextureSide,    vPos.zy * uTextureSideScale);
	vec4 ytex = texture(uTextureTopdown, vPos.xz * uTextureTopdownScale);
	vec4 ztex = texture(uTextureSide,    vPos.xy * uTextureSideScale);
	return xtex * blend.x + ytex * blend.y + ztex * blend.z;
}

float linearDepth() {
	const float uNearPlane = 1.0;
	const float uFarPlane = 100.0;
	float z = gl_FragCoord.z * 2.0 - 1.0;
	float d = (2.0 * uNearPlane * uFarPlane) / (uFarPlane + uNearPlane - z * (uFarPlane - uNearPlane));
	return d / uFarPlane;
}

void main() {
	vec3 blend = asymTriplanarBlend(vNormal);
	vec4 color = triplanarTexture(vPos, blend);
	
	Color = vec4(color.rgb, color.a);
	Position = vec4(vPos, color.a);
	Normal = vec4(vNormal, color.a);
	Depth = vec4(linearDepth(), gl_FragCoord.z, gl_FragCoord.w, color.a);
}
