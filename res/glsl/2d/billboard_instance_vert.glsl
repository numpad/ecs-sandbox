#version 450 core

uniform mat4 uProjection;
uniform mat4 uView;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in mat4 aInstanceModel;
// location 2-5 reserved by aInstanceModel

out vec2 vTexCoord;

void main() {
	vTexCoord = aTexCoord;
	gl_Position = uProjection * uView * aInstanceModel * vec4(aPosition, 1.0);
}
