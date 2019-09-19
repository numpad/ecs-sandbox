#version 450 core

uniform bool uDebugToggle;

uniform sampler2D uTextures[10];

in vec2 vTexCoord;
in vec2 vRawTexCoord;
in vec3 vColor;
flat in uint vTextureIndex;

out vec4 Color;

void main() {
	vec4 pixel = texture(uTextures[vTextureIndex], vTexCoord);
	if (pixel.r > 0.9 && pixel.g < 0.1 && pixel.b > 0.9)
		pixel = vec4(vColor, pixel.a);
	
	// debug
	if (uDebugToggle && (abs(vRawTexCoord.y * 2.0 - 1.0) > 0.9 || abs(vRawTexCoord.x * 2.0 - 1.0) > 0.9)) {
		if (vTextureIndex == 0u)
			pixel = vec4(1.0, 0.0, 0.0, 1.0);
		else if (vTextureIndex == 1u)
			pixel = vec4(0.0, 1.0, 0.0, 1.0);
		else if (vTextureIndex == 2u)
			pixel = vec4(0.0, 0.0, 1.0, 1.0);
		else if (vTextureIndex == 3u)
			pixel = vec4(1.0, 1.0, 0.0, 1.0);
		else if (vTextureIndex == 4u)
			pixel = vec4(0.0, 1.0, 1.0, 1.0);
		else if (vTextureIndex == 5u)
			pixel = vec4(1.0, 0.0, 1.0, 1.0);
		else
			pixel = vec4(1.0, 1.0, 1.0, 1.0);
	}
	
	Color = pixel; 
}
