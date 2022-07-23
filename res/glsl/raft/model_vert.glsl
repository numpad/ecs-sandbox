#version 450 core

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoord;

out vec3 vPosition;
out vec3 vNormal;
out vec3 vColor;

void main() {
	vPosition = (uModel * vec4(aPosition, 1.0)).xyz;
	vNormal = mat3(transpose(inverse(uModel))) * aNormal;
	vColor = aColor;
	gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}

