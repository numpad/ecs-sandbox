#version 450 core

uniform sampler2D uTextureSide;
uniform sampler2D uTextureTopdown;
uniform float uTextureSideScale;
uniform float uTextureTopdownScale;

in vec3 vPos;
in vec3 vNormal;

out vec4 Color;

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
	vec3 blend = triplanarBlend(vNormal);
	vec4 color = triplanarTexture(vPos, blend);
	
	Color = vec4(color.rgb, 1.0);
}
