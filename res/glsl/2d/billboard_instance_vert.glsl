#version 450 core

uniform mat4 uProjection;
uniform mat4 uView;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in mat4 aInstanceModel;
// location 2-5 reserved by aInstanceModel
layout(location = 6) in vec3 aInstanceColor;
layout(location = 7) in vec4 aTexOffsets;

out vec2 vTexCoord;
out vec3 vColor;

void main() {
	vTexCoord = aTexOffsets.xy + aTexCoord * aTexOffsets.zw;
	vColor = aInstanceColor;
	gl_Position = uProjection * uView * aInstanceModel * vec4(aPosition, 1.0);
}
