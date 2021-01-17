#version 450 core

uniform mat4 uView;
uniform mat4 uProjection;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aTexOffset;
layout(location = 2) in mat4 aInstanceModel;
// location 2-5 reserved by aInstanceModel
layout(location = 6) in uint aTextureIndex;

out vec3 vPosition;
out vec4 vTexOffset;
out mat4 vModelInv;
flat out uint vTextureIndex;

void main() {
	vec4 worldPosition = aInstanceModel * vec4(aPosition, 1.0);

	vModelInv = inverse(aInstanceModel);
	vPosition = worldPosition.xyz;
	vTexOffset = aTexOffset;
	vTextureIndex = aTextureIndex;

	gl_Position = uProjection * uView * worldPosition;
}
