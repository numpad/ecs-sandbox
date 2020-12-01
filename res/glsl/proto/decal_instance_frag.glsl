#version 450 core
#define MAX_BOUND_TEXTURES 10

uniform sampler2D uTextures[MAX_BOUND_TEXTURES];
uniform vec2 uResolution;
uniform sampler2D uTexDepth;
uniform mat4 uView;
uniform mat4 uProjection;

in vec3 vPosition;
in vec4 vTexOffset;
in mat4 vModelInv;
flat in uint vTextureIndex;

layout(location = 0) out vec4 Color;

void main() {
	vec2 texCoord = gl_FragCoord.xy / uResolution;
	vec4 worldPos = texture(uTexDepth, texCoord);

	vec4 objectPos = vModelInv * worldPos;
	if (abs(objectPos.x) > 0.5) discard;
	if (abs(objectPos.y) > 0.5) discard;
	if (abs(objectPos.z) > 0.5) discard;

	// calculate projected xz-coordinates onto surface, also flip texture x
	vec2 decalTexCoord = objectPos.xz * vec2(-1., 1.) + 0.5;

	// calculate texcoords inside subrect
	vec2 subrectTexCoord = fma(decalTexCoord, vTexOffset.zw, vTexOffset.xy);
	// get from selected texture
	vec4 color = texture(uTextures[vTextureIndex], subrectTexCoord);
	Color = color;
}
