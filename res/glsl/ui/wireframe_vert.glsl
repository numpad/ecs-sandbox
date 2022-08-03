#version 450 core


uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

layout(location = 0) in vec2 aPosition;

out vec2 vTexCoord;

void main() {
	vTexCoord = aPosition;
	gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0, 1.0);
}

