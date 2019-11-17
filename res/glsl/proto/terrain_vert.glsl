#version 450 core

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexture;

out vec3 vPos;
out vec3 vNormal;
flat out uvec2 vTexture;

void main() {
	vPos = aPos;
	vNormal = aNormal;
	vTexture = uvec2(floor(aTexture));
	gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}
