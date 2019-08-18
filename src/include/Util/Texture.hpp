#pragma once

#include <string>
#include <GL/gl3w.h>
#include <stb/stb_image.h>

class Texture {
public:
	
	Texture() = default;
	Texture(const Texture &copy) = delete;
	
	operator GLuint() const { return this->texture; };
	
	bool loadImage(std::string path);
	bool loadMemory(unsigned char *data, int width, int height, int channels = 0);
	
	void destroy();
	
	void bind();
	void unbind();
	
private:
	
	void create();
	
	int width, height, nChannels;
	
	GLuint texture = 0;
	GLint prevBoundTexture = 0;
};

