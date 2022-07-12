#version 450 core

uniform float uTime;

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec3 left;
	vec3 up;
} gs_in[];

void main() {
	float size = 0.05;
	gl_Position = gl_in[0].gl_Position + vec4(gs_in[0].left * size * 0.7, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(-gs_in[0].left * size * 0.7, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(gs_in[0].up * size, 0.0);
	EmitVertex();

	EndPrimitive();
}
