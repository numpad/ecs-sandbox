#version 450 core
#define MAX_BOUND_TEXTURES 10

uniform bool uDebugToggle;

uniform sampler2D uTextures[MAX_BOUND_TEXTURES];

in vec3 vPosition;
in vec2 vTexCoord;
in vec2 vRawTexCoord;
in vec3 vColor;
flat in uint vTextureIndex;
in vec3 vNormal;

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
	vec4 pixel = texture(uTextures[vTextureIndex], vTexCoord);
	if (pixel.r > 0.9 && pixel.g < 0.1 && pixel.b > 0.9)
		pixel = vec4(vColor, pixel.a);
	
	// debug
	if (uDebugToggle && (abs(vRawTexCoord.y * 2.0 - 1.0) > 0.9 || abs(vRawTexCoord.x * 2.0 - 1.0) > 0.9) && pixel.a < 0.1) {
		if (vTextureIndex == 0u)
			pixel = vec4(1.0, 0.0, 0.0, 1.0);
		else if (vTextureIndex == 1u)
			pixel = vec4(0.0, 1.0, 0.0, 1.0);
		else if (vTextureIndex == 2u)
			pixel = vec4(0.0, 0.0, 1.0, 1.0);
		else if (vTextureIndex == 3u)
			pixel = vec4(1.0, 1.0, 0.0, 1.0);
		else if (vTextureIndex == 4u)
			pixel = vec4(0.0, 1.0, 1.0, 1.0);
		else if (vTextureIndex == 5u)
			pixel = vec4(1.0, 0.0, 1.0, 1.0);
		else
			pixel = vec4(1.0, 1.0, 1.0, 1.0);
	}
	if (pixel.a < 0.9) discard;
	
	Color = pixel; 
	Position = vec4(vPosition, pixel.a);
	Normal = vec4(vNormal, pixel.a);
	Depth = vec4(vec3(linearDepth()), pixel.a);
}
