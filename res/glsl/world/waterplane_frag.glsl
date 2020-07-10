#version 450 core

in vec4 vWorldPosition;
in vec3 vNormal;

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Position;
layout(location = 2) out vec4 Normal;

void main() {
    Color = vec4(0.28, 0.153, 0.15, 1.0);
    Position = vWorldPosition*.5+.5;
    Normal = vec4(vNormal*.5+.5, 1.);
}
