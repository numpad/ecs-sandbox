#version 450 core

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

in vec3 aPosition;

void main() {
	
	gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}