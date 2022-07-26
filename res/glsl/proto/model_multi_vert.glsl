#version 450 core

uniform mat4 uProjection;
uniform mat4 uView;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in mat4 aModel;
// location 4-7 occupied by aModel

out vec3 vColor;

void main() {
	vColor = aColor;
	gl_Position = uProjection * uView * aModel * vec4(aPosition, 1.0);
}

