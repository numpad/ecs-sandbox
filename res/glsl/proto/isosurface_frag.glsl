#version 450 core

uniform sampler2D uTextureSide;
uniform sampler2D uTextureTopdown;
uniform float uTextureSideScale;
uniform float uTextureTopdownScale;

in vec3 vPos;
in vec3 vNormal;

out vec4 Color;

vec3 asymTriplanarBlend(vec3 normal) {
	vec3 blend = vec3(0.0);
	// sides only
	vec2 xzBlend = abs(normalize(normal.xz));
	blend.xz = max(vec2(0.0), xzBlend - 0.7);
	blend.xz /= max(0.00001, dot(blend.xz, vec2(1.0, 1.0)));
	// top
	blend.y = clamp((abs(normal.y) - 0.7 * 1.02) * 80.0, 0.0, 1.0);
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
	vec4 xtex = texture(uTextureSide, vPos.zy * uTextureSideScale);
	vec4 ytex = texture(uTextureTopdown, vPos.xz * uTextureTopdownScale);
	vec4 ztex = texture(uTextureSide, vPos.xy * uTextureSideScale);
	return xtex * blend.x + ytex * blend.y + ztex * blend.z;
}

void main() {
	vec3 blend = asymTriplanarBlend(vNormal);
	vec4 color = triplanarTexture(vPos, blend);
	
	Color = vec4(color.rgb, 1.0);
}
