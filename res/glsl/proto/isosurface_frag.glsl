#version 450 core

uniform sampler2D uTexture;
uniform float uTextureScale;

in vec3 vPos;
in vec3 vNormal;

out vec4 Color;

vec3 triplanarBlend(vec3 normal) {
	vec3 blend = abs(normal);
	blend = normalize(max(blend, 0.000001));
	float b = blend.x + blend.y + blend.z;
	return blend / b;
}

vec4 triplanarTexture(vec3 pos, vec3 blend) {
	vec4 xtex = texture(uTexture, vPos.yz * uTextureScale);
	vec4 ytex = texture(uTexture, vPos.xz * uTextureScale);
	vec4 ztex = texture(uTexture, vPos.xy * uTextureScale);
	return xtex * blend.x + ytex * blend.y + ztex * blend.z;
}

void main() {
	vec3 blend = triplanarBlend(vNormal);
	vec4 color = triplanarTexture(vPos, blend);
	
	Color = vec4(color.rgb, 1.0);
}
