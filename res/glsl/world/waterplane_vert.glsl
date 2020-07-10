#version 450 core

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

layout(location = 0) in vec3 aPosition;

out vec4 vWorldPosition;
out vec3 vNormal;

void main() {
    vWorldPosition = uModel * vec4(aPosition, 1.0);
    vNormal = vec3(0., 1., 0.);
    gl_Position = uProjection * uView * vWorldPosition;
}
