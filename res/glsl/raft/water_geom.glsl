#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec3 position;
} gs_in[];

out vec3 fNormal;

void main() {
	fNormal = cross(
		normalize(gs_in[0].position - gs_in[1].position),
		normalize(gs_in[0].position - gs_in[2].position)
	);
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
}

