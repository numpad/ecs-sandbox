#version 450 core

uniform mat4 uProjection;
uniform mat4 uView;
uniform float uTime;

layout(location = 0) in vec3 aPosition;

out VS_OUT {
	vec3 left;
	vec3 up;
} vs_out;

void main() {
	vs_out.left = (uProjection * uView * vec4(vec3(-1.0, 0.0, 0.0), 1.0)).xyz;
	vs_out.up = (uProjection * uView * vec4(vec3(0.0, 1.0, 0.0), 1.0)).xyz;
	gl_Position = uProjection * uView * vec4(aPosition, 1.0);
}
