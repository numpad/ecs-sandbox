#version 450 core

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 vPos;
out vec3 vNormal;

void main() {
	vPos = aPos;
	vNormal = aNormal;
	gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}
